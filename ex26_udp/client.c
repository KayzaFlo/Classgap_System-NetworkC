#include "common.h"

int main() {
    int sfd;
    struct sockaddr_in svaddr;
    char *msg = "Test UDP";
    char buf[BUF_SIZE];

    sfd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&svaddr, 0, sizeof(svaddr));
    svaddr.sin_family = AF_INET;
    svaddr.sin_port = htons(SV_SOCK_PORT);
    inet_pton(AF_INET, "127.0.0.1", &svaddr.sin_addr); // Conversion IP texte vers binaire [cite: 21]

    // Envoi direct sans connect() [cite: 22]
    sendto(sfd, msg, strlen(msg), 0, (struct sockaddr *)&svaddr, sizeof(svaddr));
    
    // Attente de la réponse
    ssize_t n = recvfrom(sfd, buf, BUF_SIZE - 1, 0, NULL, NULL); 
    if (n > 0) {
        buf[n] = '\0';
        printf("[UDP-Client] Echo reçu : %s\n", buf);
    }

    close(sfd);
    return 0;
}