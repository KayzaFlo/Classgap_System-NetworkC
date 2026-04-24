/* exmp2_client_multi.c
 * Client TCP pour tester le serveur multi-clients du Devoir - Problème 1.
 * Lance plusieurs connexions simultanées via fork() pour vérifier que le
 * serveur les gère bien en parallèle.
 *
 * Compile : gcc -o exmp2_client_multi exmp2_client_multi.c
 * Usage   : ./exmp2_client_multi 127.0.0.1 3
 *           (envoie 3 connexions simultanées au serveur)
 */

#include "exmp1.header.h"
#include <sys/wait.h>   /* waitpid */

/* Fonction qui gère une seule connexion TCP (utilisée par chaque enfant) */
static void run_client(const char *server_ip, int client_id)
{
    int sfd;
    struct sockaddr_in svaddr;
    ssize_t numRead;
    char buf[BUF_SIZE];
    char msg[BUF_SIZE];

    /* Message unique par client pour bien les distinguer dans les logs */
    snprintf(msg, sizeof(msg), "Bonjour depuis le client %d", client_id);

    /* --- Création de la socket TCP --- */
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* --- Adresse du serveur --- */
    memset(&svaddr, 0, sizeof(struct sockaddr_in));
    svaddr.sin_family = AF_INET;
    svaddr.sin_port   = htons(PORT);

    if (inet_pton(AF_INET, server_ip, &svaddr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    /* --- Connexion --- */
    if (connect(sfd, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_in)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("[client %d] Connecté, envoi : \"%s\"\n", client_id, msg);

    /* --- Envoi du message --- */
    if (write(sfd, msg, strlen(msg)) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    /* --- Réception de l'echo --- */
    numRead = read(sfd, buf, BUF_SIZE);
    if (numRead == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    buf[numRead] = '\0';
    printf("[client %d] Réponse reçue : \"%s\"\n", client_id, buf);

    close(sfd);
}

int main(int argc, char *argv[])
{
    int num_clients, i;
    pid_t pid;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <adresse_serveur> <nombre_de_clients>\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    num_clients = atoi(argv[2]);
    if (num_clients <= 0) {
        fprintf(stderr, "Le nombre de clients doit être > 0\n");
        exit(EXIT_FAILURE);
    }

    printf("Lancement de %d connexions simultanées vers %s:%d\n",
           num_clients, argv[1], PORT);

    /* --- Création d'un processus enfant par client --- */
    for (i = 0; i < num_clients; i++) {
        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            /* Enfant : exécute une connexion, puis termine */
            run_client(argv[1], i + 1);
            exit(EXIT_SUCCESS);
        }
        /* Parent : continue la boucle pour créer les autres enfants */
    }

    /* --- Attente de tous les enfants --- */
    for (i = 0; i < num_clients; i++) {
        waitpid(-1, NULL, 0);
    }

    printf("Tous les clients ont terminé.\n");
    exit(EXIT_SUCCESS);
}
