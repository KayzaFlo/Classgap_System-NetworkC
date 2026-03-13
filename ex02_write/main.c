#include <fcntl.h>
#include <unistd.h>

int main() {
    // O_CREAT = créer si n'existe pas, O_TRUNC = vider si existe
    int fd = open("sortie.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    write(fd, "Hello\n", 6);
    write(fd, "World\n", 6);

    close(fd);
    return 0;
}