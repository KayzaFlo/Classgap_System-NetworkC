#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("capture.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    // Après dup2, tout ce qui va sur stdout ira dans le fichier
    dup2(fd, 1);
    close(fd);

    printf("Ce texte va dans capture.txt !\n");

    return 0;
}