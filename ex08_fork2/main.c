#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        printf("Enfant : je travaille...\n");
        sleep(2); // Simule un travail
        printf("Enfant : j'ai fini !\n");
    } else {
        printf("Parent : j'attends l'enfant...\n");
        wait(NULL); // Bloque jusqu'à ce que l'enfant termine
        printf("Parent : l'enfant a fini, je continue.\n");
    }

    return 0;
}