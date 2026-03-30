/*
 * Algorithme : itérations alternées pair/impair
 *   - Itération PAIRE  : compare arr[0]/arr[1], arr[2]/arr[3], ...
 *   - Itération IMPAIRE: compare arr[1]/arr[2], arr[3]/arr[4], ...
 * Chaque comparaison/échange est effectuée par un thread dédié.
 * Le thread principal vérifie après chaque itération si le tableau
 * est trié ; si oui, il affiche le résultat et quitte.
 *
 * Compilez : gcc odd_even_sort.c -o odd_even_sort -lpthread
 * Usage    : ./odd_even_sort [valeurs...]
 *            (sans args : tableau de démonstration)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

/* ── constante globale demandée par le sujet ──────────────────────── */
#define ARRAY_SIZE 8   /* doit être pair ; modifiable ici */

/* ── données globales partagées ───────────────────────────────────── */
int  arr[ARRAY_SIZE];
long total_swaps = 0;             /* compteur global d'échanges        */

pthread_mutex_t swap_mutex = PTHREAD_MUTEX_INITIALIZER;

/* ── argument passé à chaque thread ──────────────────────────────── */
typedef struct {
    int index;       /* indice GAUCHE de la paire à comparer           */
    int *swapped;    /* flag partagé : au moins un swap dans l'itération */
    pthread_mutex_t *flag_mutex;
} PaireArgs;

/* ── thread : compare et échange une paire ────────────────────────── */
void *compare_pair(void *arg)
{
    PaireArgs *a = (PaireArgs *)arg;
    int i = a->index;

    if (arr[i] > arr[i + 1]) {
        /* échange */
        int tmp    = arr[i];
        arr[i]     = arr[i + 1];
        arr[i + 1] = tmp;

        /* incrémente le compteur global (section critique) */
        pthread_mutex_lock(&swap_mutex);
        total_swaps++;
        pthread_mutex_unlock(&swap_mutex);

        /* signale qu'un swap a eu lieu dans cette itération */
        pthread_mutex_lock(a->flag_mutex);
        *(a->swapped) = 1;
        pthread_mutex_unlock(a->flag_mutex);
    }

    return NULL;
}

/* ── vérifie si le tableau est trié ──────────────────────────────── */
int is_sorted(void)
{
    for (int i = 0; i < ARRAY_SIZE - 1; i++)
        if (arr[i] > arr[i + 1]) return 0;
    return 1;
}

/* ── affiche le tableau ───────────────────────────────────────────── */
void display_array(const char *label)
{
    printf("%s : [", label);
    for (int i = 0; i < ARRAY_SIZE; i++)
        printf("%d%s", arr[i], i < ARRAY_SIZE - 1 ? ", " : "");
    printf("]\n");
}

/* ── effectue une itération (paire ou impaire) ────────────────────── */
/*
 * Crée un thread par paire, les lance tous en parallèle,
 * puis attend leur fin avec pthread_join.
 * Retourne 1 si au moins un swap a eu lieu, 0 sinon.
 */
int iterate(int paire)
{
    /*
     * Début de la paire à traiter :
     *   paire=1 → index 0, 2, 4, ...
     *   paire=0 → index 1, 3, 5, ...
     */
    int debut = paire ? 0 : 1;

    /* Nombre de paires à comparer */
    int nb_paires = (ARRAY_SIZE - debut) / 2;

    pthread_t  *threads    = malloc(nb_paires * sizeof(pthread_t));
    PaireArgs  *args       = malloc(nb_paires * sizeof(PaireArgs));
    int         swapped    = 0;
    pthread_mutex_t flag_mutex = PTHREAD_MUTEX_INITIALIZER;

    /* Crée un thread par paire */
    for (int i = 0; i < nb_paires; i++) {
        args[i].index      = debut + i * 2;
        args[i].swapped    = &swapped;
        args[i].flag_mutex = &flag_mutex;
        pthread_create(&threads[i], NULL, compare_pair, &args[i]);
    }

    /* Attend tous les threads */
    for (int i = 0; i < nb_paires; i++)
        pthread_join(threads[i], NULL);

    pthread_mutex_destroy(&flag_mutex);
    free(threads);
    free(args);

    return swapped;
}

/* ── main ─────────────────────────────────────────────────────────── */
int main(int argc, char *argv[])
{
    /* Initialise le tableau */
    if (argc > 1) {
        int n = argc - 1;
        if (n != ARRAY_SIZE) {
            fprintf(stderr, "Erreur : fournissez exactement %d valeurs "
                            "(ou modifiez ARRAY_SIZE).\n", ARRAY_SIZE);
            return 1;
        }
        for (int i = 0; i < ARRAY_SIZE; i++)
            arr[i] = atoi(argv[i + 1]);
    } else {
        /* tableau de démonstration */
        int demo[] = {7, 3, 5, 1, 8, 2, 6, 4};
        memcpy(arr, demo, sizeof(demo));
    }

    printf("=== Tri pair-impair parallèle ===\n");
    printf("ARRAY_SIZE = %d\n\n", ARRAY_SIZE);
    display_array("Tableau initial");

    int iteration = 0;
    int paire = 1; /* commence par l'itération paire */

    while (1) {
        iteration++;
        printf("\nItération %d (%s) :\n",
               iteration, paire ? "paire" : "impaire");

        iterate(paire);
        display_array("  après");

        if (is_sorted()) {
            printf("\nTableau trié après %d itération(s).\n", iteration);
            break;
        }

        /* Alterne pair / impair */
        paire = !paire;

        /*
         * Garde-fou : le tri pair-impair converge en au plus N itérations.
         * On arrête si on dépasse pour éviter une boucle infinie en cas
         * de bug.
         */
        if (iteration > 2 * ARRAY_SIZE) {
            printf("ERREUR : pas de convergence après %d itérations.\n", iteration);
            return 1;
        }
    }

    printf("\n--- Résultat ---\n");
    display_array("Tableau final ");
    printf("Nombre de swaps : %ld\n", total_swaps);

    pthread_mutex_destroy(&swap_mutex);
    return 0;
}
