#include "common.h"

int main() {
    int sfd;
    struct sockaddr_in svaddr, claddr;
    char buf[BUF_SIZE];
    socklen_t len;

    // 1. Création de la socket UDP (SOCK_DGRAM)
    sfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    // 2. Préparation de l'adresse (Wildcard + Port)
    memset(&svaddr, 0, sizeof(svaddr));
    svaddr.sin_family = AF_INET;
    svaddr.sin_addr.s_addr = htonl(INADDR_ANY); // Écoute sur toutes les interfaces
    svaddr.sin_port = htons(SV_SOCK_PORT);      // Conversion Big-Endian indispensable

    // 3. Liaison de la socket au port
    bind(sfd, (struct sockaddr *)&svaddr, sizeof(svaddr));

    printf("[UDP-Server] En attente de datagrammes...\n");

    while (1) {
        len = sizeof(claddr);
        // 4. Réception : on récupère les données ET l'adresse de l'expéditeur
        ssize_t n = recvfrom(sfd, buf, BUF_SIZE - 1, 0, (struct sockaddr *)&claddr, &len);
        
        if (n > 0) {
            buf[n] = '\0';
            printf("[UDP-Server] Reçu de %s:%d : %s\n", inet_ntoa(claddr.sin_addr), ntohs(claddr.sin_port), buf);
            
            // 5. Réponse : on renvoie les données à l'adresse qu'on vient de recevoir
            sendto(sfd, buf, n, 0, (struct sockaddr *)&claddr, len);
        }
    }
    return 0;
}