/*
 * ex4_server.c — Devoir : serveur de gestion de fichiers concurrent
 *
 * Opérations supportées via socket Unix :
 *   CREATE <nom>          → crée un fichier vide
 *   READ   <nom>          → envoie le contenu du fichier
 *   APPEND <nom> <données>→ ajoute des données à la fin du fichier
 *
 * Protocole (texte, une commande par connexion) :
 *   Client → Serveur : "CREATE fichier.txt\n"
 *   Serveur → Client : "OK\n"  ou  "ERROR: <raison>\n"
 *
 *   Client → Serveur : "READ fichier.txt\n"
 *   Serveur → Client : "<contenu>\nEOF\n"  ou  "ERROR: ...\n"
 *
 *   Client → Serveur : "APPEND fichier.txt données à ajouter\n"
 *   Serveur → Client : "OK\n"  ou  "ERROR: ...\n"
 *
 * Concurrence : un pthread par client (comme ex3).
 * Sécurité    : un mutex par fichier (via une table de locks).
 *
 * Compilez : gcc ex4_server.c -o ex4_server -lpthread
 */

#include "common.h"

#define MAX_FILES    64
#define FILES_DIR    "/tmp/socket_files"   /* dossier de travail */

/* ── table de verrous par fichier (évite les écritures concurrentes) */
typedef struct {
    char            path[256];
    pthread_mutex_t lock;
    int             used;
} FileLock;

static FileLock  file_locks[MAX_FILES];
static pthread_mutex_t table_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Retourne (en créant si besoin) le mutex associé au chemin donné */
static pthread_mutex_t *get_file_lock(const char *path)
{
    pthread_mutex_lock(&table_mutex);

    /* Cherche une entrée existante */
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_locks[i].used && strcmp(file_locks[i].path, path) == 0) {
            pthread_mutex_unlock(&table_mutex);
            return &file_locks[i].lock;
        }
    }

    /* Crée une nouvelle entrée */
    for (int i = 0; i < MAX_FILES; i++) {
        if (!file_locks[i].used) {
            strncpy(file_locks[i].path, path, sizeof(file_locks[i].path) - 1);
            pthread_mutex_init(&file_locks[i].lock, NULL);
            file_locks[i].used = 1;
            pthread_mutex_unlock(&table_mutex);
            return &file_locks[i].lock;
        }
    }

    pthread_mutex_unlock(&table_mutex);
    return NULL; /* table pleine */
}

/* ── helpers d'envoi ────────────────────────────────────────────── */
static void send_ok(int fd)
{
    write(fd, "OK\n", 3);
}

static void send_error(int fd, const char *msg)
{
    char buf[BUF_SIZE];
    int n = snprintf(buf, sizeof(buf), "ERROR: %s\n", msg);
    write(fd, buf, n);
}

/* ── opérations fichier ─────────────────────────────────────────── */

static void op_create(int client_fd, const char *filename)
{
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", FILES_DIR, filename);

    pthread_mutex_t *lock = get_file_lock(path);
    if (!lock) { send_error(client_fd, "table de locks pleine"); return; }

    pthread_mutex_lock(lock);

    /*
     * O_CREAT | O_EXCL : échoue si le fichier existe déjà.
     * 0644 : permissions rw-r--r--.
     */
    int fd = open(path, O_CREAT | O_EXCL | O_WRONLY, 0644);
    if (fd == -1) {
        send_error(client_fd, "fichier existe déjà ou erreur");
    } else {
        close(fd);
        printf("[Serveur] CREATE %s OK\n", filename);
        send_ok(client_fd);
    }

    pthread_mutex_unlock(lock);
}

static void op_read(int client_fd, const char *filename)
{
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", FILES_DIR, filename);

    pthread_mutex_t *lock = get_file_lock(path);
    if (!lock) { send_error(client_fd, "table de locks pleine"); return; }

    pthread_mutex_lock(lock);

    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        send_error(client_fd, "fichier introuvable");
        pthread_mutex_unlock(lock);
        return;
    }

    /* Envoie le contenu bloc par bloc */
    char buf[BUF_SIZE];
    ssize_t n;
    while ((n = read(fd, buf, sizeof(buf))) > 0)
        write(client_fd, buf, n);

    close(fd);
    /* Marqueur de fin de transmission */
    write(client_fd, "\nEOF\n", 5);
    printf("[Serveur] READ %s OK\n", filename);

    pthread_mutex_unlock(lock);
}

static void op_append(int client_fd, const char *filename, const char *data)
{
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", FILES_DIR, filename);

    pthread_mutex_t *lock = get_file_lock(path);
    if (!lock) { send_error(client_fd, "table de locks pleine"); return; }

    pthread_mutex_lock(lock);

    /*
     * O_APPEND : chaque write() se fait atomiquement en fin de fichier,
     * même si plusieurs processus écrivent en même temps.
     */
    int fd = open(path, O_WRONLY | O_APPEND);
    if (fd == -1) {
        send_error(client_fd, "fichier introuvable");
        pthread_mutex_unlock(lock);
        return;
    }

    write(fd, data, strlen(data));
    write(fd, "\n", 1);
    close(fd);

    printf("[Serveur] APPEND %s : \"%s\"\n", filename, data);
    send_ok(client_fd);

    pthread_mutex_unlock(lock);
}

/* ── parse et dispatch d'une commande ──────────────────────────── */
static void traiter_commande(int client_fd, char *line)
{
    /* Supprime le '\n' final */
    line[strcspn(line, "\n")] = '\0';

    printf("[Serveur] Commande reçue : \"%s\"\n", line);

    if (strncmp(line, "CREATE ", 7) == 0) {
        op_create(client_fd, line + 7);

    } else if (strncmp(line, "READ ", 5) == 0) {
        op_read(client_fd, line + 5);

    } else if (strncmp(line, "APPEND ", 7) == 0) {
        /*
         * Format : "APPEND <nom> <données>"
         * On découpe après le 2e espace.
         */
        char *rest = line + 7;
        char *space = strchr(rest, ' ');
        if (!space) {
            send_error(client_fd, "APPEND <fichier> <données>");
            return;
        }
        *space = '\0';
        op_append(client_fd, rest, space + 1);

    } else {
        send_error(client_fd, "commande inconnue (CREATE/READ/APPEND)");
    }
}

/* ── thread par client ──────────────────────────────────────────── */
void *gerer_client(void *arg)
{
    int fd = *(int *)arg;
    free(arg);

    char buf[BUF_SIZE];
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    if (n > 0) {
        buf[n] = '\0';
        traiter_commande(fd, buf);
    }

    close(fd);
    return NULL;
}

/* ── main ─────────────────────────────────────────────────────── */
int main(void)
{
    /* Crée le dossier de travail si nécessaire */
    mkdir(FILES_DIR, 0755);

    /* Initialise la table de locks */
    memset(file_locks, 0, sizeof(file_locks));

    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    CHECK(server_fd == -1, "socket");

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    unlink(SOCKET_PATH);
    CHECK(bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1, "bind");
    CHECK(listen(server_fd, 10) == -1, "listen");

    printf("[Serveur fichiers] En écoute sur %s\n", SOCKET_PATH);
    printf("[Serveur fichiers] Fichiers stockés dans %s/\n\n", FILES_DIR);

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        CHECK(client_fd == -1, "accept");

        int *fd_ptr = malloc(sizeof(int));
        *fd_ptr = client_fd;

        pthread_t tid;
        if (pthread_create(&tid, NULL, gerer_client, fd_ptr) != 0) {
            perror("pthread_create");
            close(client_fd);
            free(fd_ptr);
        } else {
            pthread_detach(tid);
        }
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}
