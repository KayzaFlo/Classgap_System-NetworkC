/* exmp1_client.c
 * Problème 3 : client UDP.
 * Prend en argument l'adresse du serveur puis les données à envoyer.
 *
 * Compile : gcc -o exmp1_client exmp1_client.c
 * Usage   : ./exmp1_client 127.0.0.1 "hello" "world"
 */

#include "exmp1.header.h"

int main(int argc, char *argv[])
{
    int sfd;
    struct sockaddr_in svaddr;  /* adresse du serveur */
    ssize_t numRead;
    char buf[BUF_SIZE];
    int i;

    /* Au minimum : nom du programme + adresse + 1 donnée */
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <adresse_serveur> <donnee1> [donnee2 ...]\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    /* --- Création de la socket UDP --- */
    sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* --- Initialisation de l'adresse du serveur --- */
    memset(&svaddr, 0, sizeof(struct sockaddr_in));
    svaddr.sin_family = AF_INET;
    svaddr.sin_port   = htons(PORT);  /* PORT défini dans le header */

    /* Conversion de l'adresse textuelle en binaire réseau (inet_pton) */
    if (inet_pton(AF_INET, argv[1], &svaddr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    /* --- Envoi de chaque argument au serveur, affichage de la réponse --- */
    for (i = 2; i < argc; i++) {
        size_t msgLen = strlen(argv[i]);

        /* Envoi du datagramme */
        if (sendto(sfd, argv[i], msgLen, 0,
                   (struct sockaddr *) &svaddr,
                   sizeof(struct sockaddr_in)) != (ssize_t) msgLen) {
            perror("sendto");
            exit(EXIT_FAILURE);
        }

        /* Réception de la réponse (echo du serveur) */
        numRead = recvfrom(sfd, buf, BUF_SIZE, 0, NULL, NULL);
        if (numRead == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }

        /* Affichage de la réponse */
        buf[numRead] = '\0';
        printf("Réponse du serveur : %s\n", buf);
    }

    close(sfd);
    exit(EXIT_SUCCESS);
}
