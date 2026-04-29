#include <unistd.h>

typedef struct _noeud_t {
	int v; // étiquette du noeud (v pour valeur)
	struct _noeud_t* g; // adresse du noeud racine du sous-arbre gauche (ou NULL)
	struct _noeud_t* d; // adresse du noeud racine du sous-arbre droite (ou NULL)
} noeud_t;

// Fonction auxiliaire interne
// Renvoie la hauteur noire si valide, -1 si invalide
static int verifier(noeud_t* r) {
    if (r == NULL)
        return 0; // arbre vide : hauteur noire = 0, valide

    // Règle : étiquette doit être 0 ou 1
    if (r->v != 0 && r->v != 1)
        return -1;

    int hg = verifier(r->g);
    int hd = verifier(r->d);

    // Si un sous-arbre est invalide, on propage l'erreur
    if (hg == -1 || hd == -1)
        return -1;

    // Règle : les hauteurs noires doivent être égales
    if (hg != hd)
        return -1;

    // Règle : un nœud rouge ne peut avoir d'enfant rouge
    if (r->v == 1) {
        if (r->g != NULL && r->g->v == 1) return -1;
        if (r->d != NULL && r->d->v == 1) return -1;
    }

    // On renvoie la hauteur noire de ce sous-arbre
    return hg + (r->v == 0 ? 1 : 0);
}

int estRougeNoir(noeud_t* r, int* n) {
    // Règle : la racine doit être noire (ou arbre vide)
    if (r != NULL && r->v != 0)
        return 0;

    int h = verifier(r);
    if (h == -1)
        return 0;

    *n = h;
    return 1;
}

// Le piège ici c'est de ne parcourir l'arbre qu'une seule fois.
// La tentation naïve serait de vérifier chaque règle séparément avec
// des fonctions différentes, ça donne O(n²) ou pire.
// La bonne approche: une seule récursion qui vérifie tout en même
// temps et remonte le compte de nœuds noirs.
