#include <stdio.h>
#include <unistd.h>

int main() {
    fork(); // Combien de processus après cette ligne ?
    fork(); // Et après celle-ci ?
    printf("Bonjour\n");
    return 0;
}
































// Réponse : 4 "Bonjour"
// Dessine l'arbre : P0 → P1, P2 → P3