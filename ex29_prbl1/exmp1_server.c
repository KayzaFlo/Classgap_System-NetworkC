/* exmp1_server.c
 * Problème 1 : serveur UDP echo.
 * Compile : gcc -o exmp1_server exmp1_server.c
 * Usage   : ./exmp1_server
 */

#include "exmp1.header.h"

int main(void)
{
    int sfd;
    struct sockaddr_in svaddr;  /* adresse du serveur */
    struct sockaddr_in claddr;  /* adresse de l'expéditeur (remplie par recvfrom) */
    socklen_t cllen;
    ssize_t numRead;
    char buf[BUF_SIZE];

    /* --- Création de la socket UDP --- */
    sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* --- Initialisation de l'adresse serveur --- */
    memset(&svaddr, 0, sizeof(struct sockaddr_in));
    svaddr.sin_family      = AF_INET;
    svaddr.sin_addr.s_addr = htonl(INADDR_ANY); /* wildcard : toutes les interfaces */
    svaddr.sin_port        = htons(PORT);        /* port en ordre réseau */

    /* --- Liaison de la socket à l'adresse --- */
    if (bind(sfd, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_in)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    printf("Serveur UDP en écoute sur le port %d...\n", PORT);

    /* --- Boucle principale : recevoir et renvoyer indéfiniment --- */
    for (;;) {
        cllen = sizeof(struct sockaddr_in);

        /* Réception d'un datagramme */
        numRead = recvfrom(sfd, buf, BUF_SIZE, 0,
                           (struct sockaddr *) &claddr, &cllen);
        if (numRead == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }

        /* Affichage du nombre d'octets et de l'adresse de l'expéditeur */
        printf("Reçu %zd octets de %s:%d\n",
               numRead,
               inet_ntoa(claddr.sin_addr),
               ntohs(claddr.sin_port));

        /* Renvoi des données à l'expéditeur (echo) */
        if (sendto(sfd, buf, numRead, 0,
                   (struct sockaddr *) &claddr, cllen) != numRead) {
            perror("sendto");
            exit(EXIT_FAILURE);
        }
    }

    /* Jamais atteint, mais bonne pratique */
    close(sfd);
    exit(EXIT_SUCCESS);
}
