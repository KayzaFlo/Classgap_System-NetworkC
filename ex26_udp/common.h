/* common.h mis à jour */
#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>

/* --- AJOUT POUR IPV4 (Réseau) --- */
#include <netinet/in.h>   /* INDISPENSABLE pour struct sockaddr_in */
#include <arpa/inet.h>    /* Pour htons(), htonl(), inet_pton() [cite: 21] */

/* --- GARDE POUR UNIX (Local) --- */
#include <sys/un.h>       /* Pour struct sockaddr_un */

#define BUF_SIZE 1024

/* Définition du port pour IPV4 */
#define SV_SOCK_PORT 8080 
#define PORT 8080         // Pour que le client et le serveur soient d'accord

/* Définition pour Unix (si vous utilisez encore AF_UNIX) */
#define SOCKET_PATH "/tmp/cours_socket.sock"
#define SV_SOCK_PATH SOCKET_PATH // Pour corriger l'erreur dans server.pdf [cite: 105]

#endif /* COMMON_H */