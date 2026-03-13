#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        // Bloc ENFANT
        printf("Enfant : mon PID est %d\n", getpid());
        printf("Enfant : mon père a le PID %d\n", getppid());
    } else {
        // Bloc PARENT
        printf("Parent : mon PID est %d\n", getpid());
        printf("Parent : j'ai créé l'enfant %d\n", pid);
    }

    return 0;
}