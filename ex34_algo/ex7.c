#include <limits.h>
#include <stdio.h>

typedef struct _cellule_t {
	int v; // valeur contenue dans la cellule (v pour valeur)
	struct _cellule_t* s; // adresse de la cellule suivante (NULL s’il n’y en a pas)
} cellule_t;
// la liste vide est représentée par la valeur NULL

int minImpair(cellule_t* t) {
    int min = INT_MAX;
    while (t != NULL) {
        if (t->v % 2 != 0 && t->v < min)
            min = t->v;
        t = t->s;
    }
    return min;
}
// Parcours itératif de la liste, O(n) temps, O(1) espace.
// C'est le maximum d'efficacité possible, on ne peut pas
// faire moins que lire chaque élément une fois.