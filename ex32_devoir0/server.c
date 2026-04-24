/* exmp2_server.c
 * Devoir - Problème 0 : serveur TCP echo (une connexion à la fois).
 * Même logique que le serveur UDP, mais avec SOCK_STREAM (TCP).
 *
 * Compile : gcc -o exmp2_server exmp2_server.c
 * Usage   : ./exmp2_server
 */

#include "exmp1.header.h"

#define BACKLOG 5   /* taille de la file d'attente de connexions (listen) */

int main(void)
{
    int sfd, cfd;
    struct sockaddr_in svaddr;
    ssize_t numRead;
    char buf[BUF_SIZE];

    /* --- Création de la socket TCP (SOCK_STREAM au lieu de SOCK_DGRAM) --- */
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* --- Initialisation de l'adresse serveur (identique au cas UDP) --- */
    memset(&svaddr, 0, sizeof(struct sockaddr_in));
    svaddr.sin_family      = AF_INET;
    svaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    svaddr.sin_port        = htons(PORT);

    /* --- Liaison de la socket --- */
    if (bind(sfd, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_in)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    /* --- Mise en écoute (TCP uniquement) --- */
    if (listen(sfd, BACKLOG) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Serveur TCP en écoute sur le port %d...\n", PORT);

    /* --- Boucle principale : accepter une connexion, l'echo-er, fermer --- */
    for (;;) {
        /* Acceptation d'une connexion entrante -> nouvelle socket cfd */
        cfd = accept(sfd, NULL, NULL);
        if (cfd == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        /* Lecture des données envoyées par le client */
        numRead = read(cfd, buf, BUF_SIZE);
        if (numRead == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        /* Affichage sur stdout */
        printf("Reçu %zd octets : ", numRead);
        write(STDOUT_FILENO, buf, numRead);
        printf("\n");

        /* Renvoi des mêmes données au client (echo) */
        if (write(cfd, buf, numRead) != numRead) {
            perror("write");
            exit(EXIT_FAILURE);
        }

        /* Fermeture de la socket client avant d'accepter la suivante */
        if (close(cfd) == -1) {
            perror("close cfd");
            exit(EXIT_FAILURE);
        }
    }

    close(sfd);
    exit(EXIT_SUCCESS);
}
