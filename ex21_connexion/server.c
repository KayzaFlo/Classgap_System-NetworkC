/*
 * ex1_server.c — Exercice 1 : serveur écho basique
 *
 * Accepte un client à la fois, lit un message, le renvoie tel quel.
 * Illustre le cycle complet : socket → bind → listen → accept → read/write → close
 *
 * Compilez : gcc ex1_server.c -o ex1_server
 * Lancez   : ./ex1_server   (dans un terminal)
 *            ./ex1_client   (dans un autre terminal)
 */

#include "common.h"

int main(void)
{
    /* ── 1. socket() ─────────────────────────────────────────────────
     * Crée un fd de type socket.
     * AF_UNIX      = domaine Unix (communication locale, pas réseau)
     * SOCK_STREAM  = flux ordonné et fiable (équivalent TCP en local)
     * 0            = protocole par défaut pour ce type
     * Retourne : un fd (entier) >= 0, ou -1 en cas d'erreur.
     */
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    CHECK(server_fd == -1, "socket");

    /* ── 2. bind() ───────────────────────────────────────────────────
     * Attache le fd à une adresse — ici un chemin sur le disque.
     * struct sockaddr_un contient :
     *   sun_family : AF_UNIX
     *   sun_path   : chemin du fichier socket (max 108 caractères)
     * Retourne : 0 si OK, -1 en cas d'erreur.
     */
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    /* Supprime un éventuel fichier socket restant d'une exécution précédente */
    unlink(SOCKET_PATH);

    CHECK(bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1, "bind");

    /* ── 3. listen() ─────────────────────────────────────────────────
     * Met le socket en mode écoute passive.
     * Le 2e argument (backlog) = taille max de la file d'attente de
     * connexions entrantes non encore traitées par accept().
     * Retourne : 0 si OK, -1 en cas d'erreur.
     */
    CHECK(listen(server_fd, 5) == -1, "listen");

    printf("[Serveur] En écoute sur %s\n", SOCKET_PATH);

    /* ── Boucle principale : un client à la fois ─────────────────── */
    while (1) {

        /* ── 4. accept() ─────────────────────────────────────────────
         * BLOQUE jusqu'à l'arrivée d'un client.
         * Retourne un NOUVEAU fd dédié à cette connexion.
         * server_fd reste intact pour le prochain accept().
         * Les 2e et 3e arguments (adresse et taille) peuvent être NULL
         * si on n'a pas besoin de savoir qui s'est connecté.
         */
        int client_fd = accept(server_fd, NULL, NULL);
        CHECK(client_fd == -1, "accept");

        printf("[Serveur] Client connecté (fd=%d)\n", client_fd);

        /* ── 5. read() / write() ─────────────────────────────────────
         * Identiques à ceux d'un fichier ou d'un pipe.
         * read() bloque jusqu'à réception de données ou fermeture du client.
         * Retourne le nombre d'octets lus, 0 si le client a fermé.
         */
        char buf[BUF_SIZE];
        ssize_t n = read(client_fd, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("[Serveur] Reçu : \"%s\"\n", buf);

            /* Renvoie le message tel quel (écho) */
            write(client_fd, buf, n);
        }

        /* ── 6. close() ──────────────────────────────────────────────
         * Ferme la connexion avec CE client.
         * server_fd reste ouvert — on reboucle sur accept().
         */
        close(client_fd);
        printf("[Serveur] Client déconnecté, en attente du suivant...\n\n");
    }

    /* Nettoyage (jamais atteint ici, mais bonne pratique) */
    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}
