/*
 * ex2_server.c — Exercice 2 : serveur de conversion minuscule → majuscule
 *
 * Protocole :
 *   1. Le client envoie UN caractère latin minuscule.
 *   2. Le serveur l'affiche et répond avec sa version majuscule.
 *   3. Le client affiche la réponse et se déconnecte.
 *   4. Le serveur reboucle sur accept().
 *
 * Compilez : gcc ex2_server.c -o ex2_server
 */

#include "common.h"
#include <ctype.h>   /* toupper() */

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
    CHECK(listen(server_fd, 5) == -1, "listen");

    printf("[Serveur majuscule] En écoute sur %s\n\n", SOCKET_PATH);

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        CHECK(client_fd == -1, "accept");

        /*
         * Lit exactement 1 caractère.
         * On utilise read() directement — le protocole est fixe.
         */
        char c;
        ssize_t n = read(client_fd, &c, 1);

        if (n == 1 && islower((unsigned char)c)) {
            printf("[Serveur] Reçu : '%c' → répond : '%c'\n", c, toupper(c));

            char upper = (char)toupper((unsigned char)c);
            write(client_fd, &upper, 1);
        } else {
            /* Caractère invalide : renvoie '?' */
            printf("[Serveur] Caractère invalide reçu.\n");
            char err = '?';
            write(client_fd, &err, 1);
        }

        close(client_fd);
        printf("[Serveur] Client traité, en attente du suivant...\n\n");
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}
