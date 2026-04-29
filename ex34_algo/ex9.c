#include <unistd.h>      /* close, read, write */

typedef struct _noeud_t {
	int v; // étiquette du noeud (v pour valeur)
	struct _noeud_t* g; // adresse du noeud racine du sous-arbre gauche (ou NULL)
	struct _noeud_t* d; // adresse du noeud racine du sous-arbre droite (ou NULL)
} noeud_t;

noeud_t* allouerNoeud(int v,noeud_t* g,noeud_t* d) {}

void curryfie(noeud_t** r) {
    if (*r == NULL)           // arbre vide → rien à faire
        return;

    noeud_t* racine = *r;

    // Cas : feuille (pas d'enfants) → inchangée
	if (racine->g == NULL && racine->d == NULL) {
		return 0;
	}

	// On curryfie les sous-arbres d'abord
	curryfie(&(racine->g));
	curryfie(&(racine->d));

	if (racine->d == NULL) {
		// Un seul sous-arbre à gauche → on enveloppe dans un -1
        // Résultat :   -1
        //              x  A'
        noeud_t* nouveau = allouerNoeud(-1, racine, NULL);
        *r = nouveau;
    }
	else if (racine->g == NULL) {
		// Un seul sous-arbre à droite → même chose
        noeud_t* nouveau = allouerNoeud(-1, racine, NULL);
        *r = nouveau;
	}
	else {
        // Deux sous-arbres → double enveloppement
        // Résultat :     -1
        //             -1    B'
        //           x   A'
		noeud_t* interne = allouerNoeud(-1, racine, NULL);
		// on détache B' de la racine pour le mettre à droite du -1 externe
		noeud_t* bd = racine->d;
		racine->d = NULL;
		interne->d = racine->g;
		racine->g = NULL;
		noeud_t* externe = allouerNoeud(-1, interne, bd);
		*r = externe;
	}
}

// Récursion post-ordre: 
// on curryfie d'abord les sous-arbres, puis on restructure la racine.
// O(n) temps, O(h) espace.
