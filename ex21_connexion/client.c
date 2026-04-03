/*
 * ex1_client.c — Exercice 1 : client écho basique
 *
 * Se connecte au serveur, envoie un message, affiche la réponse.
 *
 * Compilez : gcc ex1_client.c -o ex1_client
 * Usage    : ./ex1_client [message]
 */

#include "common.h"

int main(int argc, char *argv[])
{
    const char *message = (argc > 1) ? argv[1] : "bonjour serveur";

    /* ── 1. socket() ─────────────────────────────────────────────────
     * Le client crée aussi son propre fd socket.
     * Même arguments que le serveur : AF_UNIX, SOCK_STREAM.
     */
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    CHECK(fd == -1, "socket");

    /* ── 2. connect() ────────────────────────────────────────────────
     * Cherche le fichier SOCKET_PATH sur le disque et demande
     * une connexion au serveur qui est en écoute dessus.
     * BLOQUE jusqu'à ce que le serveur appelle accept().
     * Retourne : 0 si OK, -1 en cas d'erreur (serveur pas lancé ?).
     *
     * Pas de bind() côté client : le kernel lui assigne un nom interne
     * automatiquement, on n'a pas besoin de le connaître.
     */
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    CHECK(connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1, "connect");

    printf("[Client] Connecté à %s\n", SOCKET_PATH);

    /* ── 3. write() ──────────────────────────────────────────────────
     * Envoie le message au serveur.
     */
    write(fd, message, strlen(message));
    printf("[Client] Envoyé : \"%s\"\n", message);

    /* ── 4. read() ───────────────────────────────────────────────────
     * Attend et lit la réponse du serveur.
     */
    char buf[BUF_SIZE];
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    if (n > 0) {
        buf[n] = '\0';
        printf("[Client] Réponse : \"%s\"\n", buf);
    }

    /* ── 5. close() ──────────────────────────────────────────────────
     * Ferme la connexion. Le serveur verra read() retourner 0.
     */
    close(fd);
    printf("[Client] Connexion fermée.\n");
    return 0;
}
