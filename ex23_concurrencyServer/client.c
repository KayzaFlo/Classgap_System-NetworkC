/*
 * ex3_client.c — Exercice 3 : client multi-caractères
 *
 * Envoie plusieurs caractères en boucle pour tester le serveur concurrent.
 * Lancez plusieurs instances en parallèle pour tester la concurrence.
 *
 * Compilez : gcc ex3_client.c -o ex3_client
 * Usage    : ./ex3_client [id] [nbr_messages]
 *
 * Test concurrence :
 *   ./ex3_client 1 5 &
 *   ./ex3_client 2 5 &
 *   ./ex3_client 3 5 &
 */

#include "common.h"
#include <time.h>

int main(int argc, char *argv[])
{
    int id  = (argc > 1) ? atoi(argv[1]) : 1;
    int nb  = (argc > 2) ? atoi(argv[2]) : 3;

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    CHECK(fd == -1, "socket");

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    CHECK(connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1, "connect");
    printf("[Client %d] Connecté.\n", id);

    /* Génère des lettres à partir de 'a' + id */
    for (int i = 0; i < nb; i++) {
        char c = (char)('a' + (id + i) % 26);

        write(fd, &c, 1);
        printf("[Client %d] Envoyé '%c'\n", id, c);

        char response;
        read(fd, &response, 1);
        printf("[Client %d] Réponse '%c'\n", id, response);

        usleep(200000); /* 200ms entre chaque message */
    }

    close(fd);
    printf("[Client %d] Terminé.\n", id);
    return 0;
}
