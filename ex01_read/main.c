#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    int fd = open("exemple.txt", O_RDONLY);
    if (fd == -1) {
        perror("Erreur open");
        return 1;
    }

    char buf[1];
    ssize_t n;

    // On lit octet par octet jusqu'à la fin du fichier
    while ((n = read(fd, buf, 1)) > 0) {
        write(1, buf, 1); // 1 = stdout
    }

    close(fd);
    return 0;
}