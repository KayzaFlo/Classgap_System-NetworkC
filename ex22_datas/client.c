/*
 * ex2_client.c — Exercice 2 : client de conversion minuscule → majuscule
 *
 * Compilez : gcc ex2_client.c -o ex2_client
 * Usage    : ./ex2_client [caractere]
 */

#include "common.h"
#include <ctype.h>

int main(int argc, char *argv[])
{
    char c;

    if (argc > 1 && islower((unsigned char)argv[1][0])) {
        c = argv[1][0];
    } else {
        /* Demande à l'utilisateur */
        printf("Entrez un caractère minuscule : ");
        scanf(" %c", &c);
        if (!islower((unsigned char)c)) {
            fprintf(stderr, "Erreur : '%c' n'est pas une minuscule.\n", c);
            return 1;
        }
    }

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    CHECK(fd == -1, "socket");

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    CHECK(connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1, "connect");

    printf("[Client] Envoi de '%c' au serveur...\n", c);
    write(fd, &c, 1);

    char response;
    ssize_t n = read(fd, &response, 1);
    if (n == 1)
        printf("[Client] Réponse reçue : '%c'\n", response);

    close(fd);
    printf("[Client] Connexion fermée.\n");
    return 0;
}
