#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    int fd = open("lettres.txt", O_RDONLY);
    char buf[1];

    // Aller au 3ème caractère (index 2)
    lseek(fd, 2, SEEK_SET);
    read(fd, buf, 1);
    printf("Caractère en position 2 : %c\n", buf[0]); // C

    // Aller au dernier caractère
    lseek(fd, -1, SEEK_END);
    read(fd, buf, 1);
    printf("Dernier caractère : %c\n", buf[0]); // E

    // Connaître la taille du fichier
    off_t taille = lseek(fd, 0, SEEK_END);
    printf("Taille du fichier : %ld octets\n", taille); // 5

    close(fd);
    return 0;
}