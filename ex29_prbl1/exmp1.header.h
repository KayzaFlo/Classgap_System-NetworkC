/* exmp1.header.h
 * En-tête commun pour l'application UDP (exercice 0)
 * Inclut toutes les librairies nécessaires et les constantes partagées.
 */

#ifndef EXMP1_HEADER_H
#define EXMP1_HEADER_H

#include <stdio.h>       /* printf, fprintf, perror */
#include <stdlib.h>      /* exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h>      /* memset, strlen, strcpy */
#include <unistd.h>      /* close, read, write */
#include <arpa/inet.h>   /* inet_pton, inet_ntoa, htons, htonl, ntohs */
#include <sys/socket.h>  /* socket, bind, sendto, recvfrom, sockaddr */
#include <sys/types.h>   /* ssize_t */

/* Port d'écoute du serveur (même valeur utilisée par le client) */
#define PORT     8080

/* Taille maximale d'un datagramme / buffer de lecture */
#define BUF_SIZE 1024

#endif /* EXMP1_HEADER_H */
