#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int fd[2];
    pipe(fd); // fd[0] = lecture, fd[1] = écriture

    if (fork() == 0) {
        // ENFANT : écrit dans le pipe
        close(fd[0]); // Ferme ce qu'il n'utilise pas
        int valeur = 99;
        write(fd[1], &valeur, sizeof(int));
        close(fd[1]);
    } else {
        // PARENT : lit depuis le pipe
        close(fd[1]); // Ferme ce qu'il n'utilise pas
        int valeur;
        read(fd[0], &valeur, sizeof(int));
        printf("Parent reçu : %d\n", valeur);
        close(fd[0]);
        wait(NULL);
    }

    return 0;
}