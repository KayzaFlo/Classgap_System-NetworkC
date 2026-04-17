#include "common.h"

int main() {
    int sfd;
    struct sockaddr_in svaddr;
    char buf[BUF_SIZE];

    sfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&svaddr, 0, sizeof(svaddr));
    svaddr.sin_family = AF_INET;
    svaddr.sin_port = htons(SV_SOCK_PORT);
    inet_pton(AF_INET, "127.0.0.1", &svaddr.sin_addr);

    // 1. Établissement de la connexion 
    if (connect(sfd, (struct sockaddr *)&svaddr, sizeof(svaddr)) == -1) {
        perror("Connexion échouée");
        exit(1);
    }

    // 2. Envoi et réception [cite: 55, 63]
    write(sfd, "Hello TCP", 9);
    ssize_t n = read(sfd, buf, BUF_SIZE - 1);
    if (n > 0) {
        buf[n] = '\0';
        printf("[TCP-Client] Réponse : %s\n", buf);
    }

    close(sfd);
    return 0;
}