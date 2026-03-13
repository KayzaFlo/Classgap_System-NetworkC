#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define BUFFER_SIZE 10

int main() {
    int fd = open("exemple.txt", O_RDONLY);
    if (fd == -1) {
        perror("Erreur open");
        return 1;
    }

    ssize_t n = BUFFER_SIZE;
    char buf[n];

    // On lit octet par octet jusqu'à la fin du fichier
    while ((n = read(fd, buf, n)) > 0) {
        write(1, buf, n); // 1 = stdout
    }

    close(fd);
    return 0;
}