#include "common.h"

int main() {
    int sfd, cfd;
    struct sockaddr_in svaddr;
    char buf[BUF_SIZE];

    // 1. Socket de flux (SOCK_STREAM) [cite: 75]
    sfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&svaddr, 0, sizeof(svaddr));
    svaddr.sin_family = AF_INET;
    svaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    svaddr.sin_port = htons(SV_SOCK_PORT);

    bind(sfd, (struct sockaddr *)&svaddr, sizeof(svaddr));

    // 2. Mise en écoute (Backlog = 5) [cite: 88]
    listen(sfd, 5);
    printf("[TCP-Server] En attente de connexion...\n");

    while (1) {
        // 3. Acceptation : crée une NOUVELLE socket (cfd) dédiée au client [cite: 92]
        cfd = accept(sfd, NULL, NULL); 
        printf("[TCP-Server] Client connecté !\n");

        // 4. Communication via read/write [cite: 96, 103]
        ssize_t n = read(cfd, buf, BUF_SIZE - 1);
        if (n > 0) {
            buf[n] = '\0';
            write(cfd, "Message bien reçu", 18);
        }

        // 5. Fermeture de la session client, mais le serveur (sfd) continue [cite: 108]
        close(cfd);
    }
    return 0;
}