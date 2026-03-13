#include <stdio.h>
#include <sys/types.h>

int main() {
    FILE *f = fopen("lettres.txt", "r");

    // Aller à la fin pour connaître la taille
    fseeko(f, 0, SEEK_END);
    off_t taille = ftello(f); // Position actuelle = taille du fichier
    printf("Taille : %ld\n", taille);

    // Revenir au début
    fseeko(f, 0, SEEK_SET);

    // Aller au 2ème caractère
    fseeko(f, 2, SEEK_SET);
    char buf[1];
    fread(buf, 1, 1, f);
    printf("Caractère : %c\n", buf[0]);

    fclose(f);
    return 0;
}