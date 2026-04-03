/*
 * ex3_server.c — Exercice 3 : serveur concurrent (multi-clients via threads)
 *
 * Pour chaque client accepté, un thread est créé pour le gérer.
 * Le thread principal ne fait que accept() en boucle.
 *
 * Protocole identique à l'exercice 2 (minuscule → majuscule),
 * mais N clients peuvent être servis simultanément.
 *
 * Compilez : gcc ex3_server.c -o ex3_server -lpthread
 */

#include "common.h"
#include <ctype.h>

/*
 * Argument passé au thread : le fd du client.
 * On alloue un int sur le heap pour éviter la race condition
 * classique (passer &client_fd depuis la stack de main()).
 */
typedef struct {
    int client_fd;
    int client_num;   /* numéro d'ordre pour les logs */
} ClientArgs;

/* ── fonction exécutée par chaque thread client ───────────────────── */
void *gerer_client(void *arg)
{
    ClientArgs *a = (ClientArgs *)arg;
    int fd  = a->client_fd;
    int num = a->client_num;
    free(a);  /* libère l'argument alloué par main */

    printf("[Thread %d] Client connecté (fd=%d)\n", num, fd);

    /*
     * Boucle : le client peut envoyer plusieurs caractères
     * tant qu'il ne ferme pas la connexion.
     */
    char c;
    ssize_t n;
    while ((n = read(fd, &c, 1)) > 0) {
        printf("[Thread %d] Reçu '%c'", num, c);

        char response;
        if (islower((unsigned char)c)) {
            response = (char)toupper((unsigned char)c);
            printf(" → '%c'\n", response);
        } else {
            response = '?';
            printf(" → invalide\n");
        }
        write(fd, &response, 1);
    }

    printf("[Thread %d] Client déconnecté.\n\n", num);
    close(fd);
    return NULL;
}

/* ── main : accept en boucle, thread pour chaque client ─────────── */
int main(void)
{
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    CHECK(server_fd == -1, "socket");

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    unlink(SOCKET_PATH);
    CHECK(bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1, "bind");

    /*
     * backlog=10 : jusqu'à 10 connexions en attente dans la file
     * avant que le kernel commence à les refuser.
     */
    CHECK(listen(server_fd, 10) == -1, "listen");

    printf("[Serveur concurrent] En écoute sur %s\n", SOCKET_PATH);
    printf("[Serveur concurrent] Chaque client est géré dans son propre thread.\n\n");

    int client_count = 0;

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        CHECK(client_fd == -1, "accept");

        client_count++;

        /*
         * Alloue les arguments sur le heap — indispensable !
         * Si on passait &client_fd (variable locale de la boucle),
         * la valeur serait écrasée au prochain tour avant que le
         * thread ait eu le temps de la lire → race condition.
         */
        ClientArgs *args = malloc(sizeof(ClientArgs));
        args->client_fd  = client_fd;
        args->client_num = client_count;

        pthread_t tid;
        if (pthread_create(&tid, NULL, gerer_client, args) != 0) {
            perror("pthread_create");
            close(client_fd);
            free(args);
            continue;
        }

        /*
         * pthread_detach : le thread libère ses ressources tout seul
         * quand il termine. On n'a pas besoin de pthread_join ici
         * car main() ne veut pas attendre chaque thread — il reboucle
         * immédiatement sur accept().
         */
        pthread_detach(tid);
        printf("[Main] Thread %d lancé pour le client fd=%d\n", client_count, client_fd);
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}
