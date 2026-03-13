#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid1 = fork(); // Premier enfant

    if (pid1 == 0) {
        sleep(3);
        printf("Enfant 1 terminé\n");
        return 0;
    }

    pid_t pid2 = fork(); // Deuxième enfant

    if (pid2 == 0) {
        sleep(1);
        printf("Enfant 2 terminé\n");
        return 0;
    }

    // Parent attend uniquement l'enfant 2
    waitpid(pid2, NULL, 0);
    printf("Parent : enfant 2 fini, j'attends enfant 1\n");
    waitpid(pid1, NULL, 0);
    printf("Parent : tout le monde a fini\n");

    return 0;
}