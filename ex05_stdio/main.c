#include <stdio.h>

int main() {
    FILE *f = fopen("exemple.txt", "r");
    if (f == NULL) {
        perror("Erreur fopen");
        return 1;
    }

    char buf[1];
    while (fread(buf, 1, 1, f) > 0) {
        fwrite(buf, 1, 1, stdout);
    }

    fclose(f);
    return 0;
}