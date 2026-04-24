/* exmp2_server_multi.c
 * Devoir - Problème 1 : serveur TCP multi-clients via fork().
 * Pour chaque nouvelle connexion, on crée un processus enfant qui gère
 * l'échange, pendant que le parent continue d'accepter d'autres connexions.
 *
 * Compile : gcc -o exmp2_server_multi exmp2_server_multi.c
 * Usage   : ./exmp2_server_multi
 */

#include "exmp1.header.h"
#include <sys/wait.h>   /* waitpid, WNOHANG */

#define BACKLOG 5

int main(void)
{
    int sfd, cfd;
    struct sockaddr_in svaddr;
    ssize_t numRead;
    char buf[BUF_SIZE];
    pid_t pid;

    /* --- Création de la socket TCP --- */
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* --- Initialisation de l'adresse serveur --- */
    memset(&svaddr, 0, sizeof(struct sockaddr_in));
    svaddr.sin_family      = AF_INET;
    svaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    svaddr.sin_port        = htons(PORT);

    /* --- Liaison --- */
    if (bind(sfd, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_in)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    /* --- Écoute --- */
    if (listen(sfd, BACKLOG) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Serveur TCP multi-clients en écoute sur le port %d...\n", PORT);

    /* --- Boucle principale --- */
    for (;;) {
        /* Nettoyage des processus enfants terminés (évite les zombies) */
        waitpid(-1, NULL, WNOHANG);

        /* Acceptation d'une nouvelle connexion */
        cfd = accept(sfd, NULL, NULL);
        if (cfd == -1) {
            perror("accept");
            continue; /* on continue plutôt que de quitter */
        }

        /* Création d'un processus enfant pour gérer ce client */
        pid = fork();
        if (pid == -1) {
            perror("fork");
            close(cfd);
            continue;
        }

        if (pid == 0) {
            /* === Processus ENFANT : gère la connexion client === */

            /* L'enfant n'a pas besoin de la socket d'écoute */
            close(sfd);

            /* Lecture des données du client */
            numRead = read(cfd, buf, BUF_SIZE);
            if (numRead == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }

            printf("[enfant %d] Reçu %zd octets : ", getpid(), numRead);
            write(STDOUT_FILENO, buf, numRead);
            printf("\n");

            /* Echo au client */
            if (write(cfd, buf, numRead) != numRead) {
                perror("write");
                exit(EXIT_FAILURE);
            }

            close(cfd);
            exit(EXIT_SUCCESS);

        } else {
            /* === Processus PARENT : ferme sa copie de cfd et reboucle === */
            close(cfd);
        }
    }

    close(sfd);
    exit(EXIT_SUCCESS);
}
