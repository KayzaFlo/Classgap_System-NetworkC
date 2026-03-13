#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        // L'enfant se transforme en "ls -l"
        execlp("ls", "ls", "-l", NULL);
        // Si on arrive ici, exec a échoué
        perror("Erreur exec");
        return 1;
    } else {
        wait(NULL);
        printf("Parent : ls a terminé\n");
    }

    return 0;
}