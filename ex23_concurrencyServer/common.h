/*
 * common.h — en-tête partagé entre tous les exemples de sockets Unix
 *
 * Contient : les includes nécessaires, le chemin du socket, et
 * une macro d'erreur pratique.
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Sockets Unix */
#include <sys/socket.h>   /* socket(), bind(), listen(), accept(), connect() */
#include <sys/un.h>       /* struct sockaddr_un — adresse de socket Unix     */

/* Threads (exemples 3 et 4) */
#include <pthread.h>

/* Fichiers (exemple 4 — serveur de fichiers) */
#include <fcntl.h>
#include <sys/stat.h>

/* ── chemin du fichier socket sur le disque ──────────────────────── */
#define SOCKET_PATH "/tmp/cours_socket.sock"

/* Taille maximale d'un message échangé */
#define BUF_SIZE 1024

/*
 * Macro d'erreur : affiche le message et quitte proprement.
 * Utilisation : CHECK(bind(fd, ...) == -1, "bind");
 */
#define CHECK(cond, msg)   \
    do {                   \
        if (cond) {        \
            perror(msg);   \
            exit(1);       \
        }                  \
    } while (0)

#endif
