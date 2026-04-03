/*
 * ex4_client.c — Devoir : client de gestion de fichiers
 *
 * Illustre les 3 opérations : CREATE, READ, APPEND.
 * Peut aussi être utilisé en ligne de commande.
 *
 * Compilez : gcc ex4_client.c -o ex4_client
 * Usage    :
 *   ./ex4_client CREATE notes.txt
 *   ./ex4_client APPEND notes.txt "première ligne"
 *   ./ex4_client APPEND notes.txt "deuxième ligne"
 *   ./ex4_client READ   notes.txt
 *
 * Démonstration auto : ./ex4_client demo
 */

#include "common.h"

/* ── connexion au serveur, envoi d'une commande, lecture réponse ── */
static void envoyer_commande(const char *commande)
{
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    CHECK(fd == -1, "socket");

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    CHECK(connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1, "connect");

    printf("[Client] --> %s\n", commande);

    /* Envoie la commande */
    write(fd, commande, strlen(commande));

    /* Lit et affiche la réponse jusqu'à fermeture ou "EOF\n" */
    char buf[BUF_SIZE];
    ssize_t n;
    printf("[Client] <-- ");
    int got_response = 0;

    while ((n = read(fd, buf, sizeof(buf) - 1)) > 0) {
        buf[n] = '\0';
        printf("%s", buf);
        got_response = 1;
        /* Le serveur ferme après avoir envoyé "EOF\n" ou "OK\n" */
        if (strstr(buf, "EOF") || strstr(buf, "OK") || strstr(buf, "ERROR"))
            break;
    }
    if (!got_response) printf("(pas de réponse)");
    printf("\n");

    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc == 2 && strcmp(argv[1], "demo") == 0) {
        /* ── Démonstration complète des 3 opérations ── */
        printf("=== Démonstration ex4 : gestion de fichiers ===\n\n");

        printf("--- 1. Création du fichier ---\n");
        envoyer_commande("CREATE demo.txt");

        printf("\n--- 2. Ajout de contenu ---\n");
        envoyer_commande("APPEND demo.txt Première ligne ajoutée par le client");
        envoyer_commande("APPEND demo.txt Deuxième ligne ajoutée par le client");
        envoyer_commande("APPEND demo.txt Troisième ligne - test concurrent");

        printf("\n--- 3. Lecture du contenu ---\n");
        envoyer_commande("READ demo.txt");

        printf("\n--- 4. Lecture d'un fichier inexistant ---\n");
        envoyer_commande("READ inexistant.txt");

        printf("\n--- 5. Commande invalide ---\n");
        envoyer_commande("DELETE demo.txt");

        return 0;
    }

    /* ── Mode ligne de commande ── */
    if (argc < 3) {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  %s CREATE <fichier>\n", argv[0]);
        fprintf(stderr, "  %s READ   <fichier>\n", argv[0]);
        fprintf(stderr, "  %s APPEND <fichier> <données>\n", argv[0]);
        fprintf(stderr, "  %s demo\n", argv[0]);
        return 1;
    }

    char commande[BUF_SIZE];

    if (strcmp(argv[1], "CREATE") == 0) {
        snprintf(commande, sizeof(commande), "CREATE %s", argv[2]);

    } else if (strcmp(argv[1], "READ") == 0) {
        snprintf(commande, sizeof(commande), "READ %s", argv[2]);

    } else if (strcmp(argv[1], "APPEND") == 0) {
        if (argc < 4) {
            fprintf(stderr, "APPEND nécessite un 3e argument : les données.\n");
            return 1;
        }
        snprintf(commande, sizeof(commande), "APPEND %s %s", argv[2], argv[3]);

    } else {
        fprintf(stderr, "Commande inconnue : %s\n", argv[1]);
        return 1;
    }

    envoyer_commande(commande);
    return 0;
}
