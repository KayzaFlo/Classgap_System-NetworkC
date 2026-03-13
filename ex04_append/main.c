#include <fcntl.h>
#include <unistd.h>

int main() {
    // O_APPEND = toujours écrire à la fin
    int fd = open("journal.txt", O_WRONLY | O_APPEND);

    write(fd, "Nouvelle ligne\n", 15);

    close(fd);
    return 0;
}