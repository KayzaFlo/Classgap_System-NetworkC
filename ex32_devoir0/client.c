/* exmp2_client.c
 * Client TCP simple pour tester le serveur du Devoir - Problème 0.
 * Prend en argument l'adresse du serveur et les données à envoyer.
 *
 * Compile : gcc -o exmp2_client exmp2_client.c
 * Usage   : ./exmp2_client 127.0.0.1 "hello serveur"
 */

#include "exmp1.header.h"

int main(int argc, char *argv[])
{
    int sfd;
    struct sockaddr_in svaddr;
    ssize_t numRead;
    char buf[BUF_SIZE];

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <adresse_serveur> <message>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* --- Création de la socket TCP --- */
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* --- Initialisation de l'adresse du serveur --- */
    memset(&svaddr, 0, sizeof(struct sockaddr_in));
    svaddr.sin_family = AF_INET;
    svaddr.sin_port   = htons(PORT);

    if (inet_pton(AF_INET, argv[1], &svaddr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    /* --- Connexion au serveur --- */
    if (connect(sfd, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_in)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("Connecté au serveur %s:%d\n", argv[1], PORT);

    /* --- Envoi du message --- */
    if (write(sfd, argv[2], strlen(argv[2])) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    /* --- Réception de l'echo du serveur --- */
    numRead = read(sfd, buf, BUF_SIZE);
    if (numRead == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    buf[numRead] = '\0';
    printf("Réponse du serveur : %s\n", buf);

    close(sfd);
    exit(EXIT_SUCCESS);
}
