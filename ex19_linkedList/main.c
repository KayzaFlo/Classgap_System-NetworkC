/*
 * Liste chaînée partagée entre un thread producteur (écrit des values
 * aléatoires depuis /dev/urandom) et un thread consommateur (lit et
 * affiche les nouveaux noeuds).
 *
 * Compilez : gcc linkedLists.c -o linkedLists -lpthread
 * Usage    : ./linkedLists [--no-mutex]
 *
 * Sans --no-mutex : version correcte avec mutex
 * Avec --no-mutex : version cassée pour observer la race condition
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define NB_AJOUTS     10      /* nombre de noeuds que le producteur va ajouter */
#define SLEEP_PROD_US 300000  /* 300ms entre chaque ajout */
#define SLEEP_CONS_US 150000  /* 150ms entre chaque vérification */

/* ── structure de la liste ────────────────────────────────────────── */
typedef struct Node {
    int          value;
    struct Node *next;
} Node;

typedef struct {
    Node           *head;
    int             tail;
    pthread_mutex_t mutex;
} Liste;

/* ── variable globale : indique si on utilise le mutex ────────────── */
int use_mutex = 1;

/* ── opérations sur la liste ──────────────────────────────────────── */

Liste *list_create(void)
{
    Liste *l = malloc(sizeof(Liste));
    l->head   = NULL;
    l->tail = 0;
    pthread_mutex_init(&l->mutex, NULL);
    return l;
}

/*
 * Ajoute un noeud EN FIN de liste.
 * La modification de tail DOIT être atomique avec l'ajout du noeud,
 * sinon le consommateur peut lire une tail incohérente.
 */
void list_add(Liste *l, int value)
{
    Node *nouveau = malloc(sizeof(Node));
    nouveau->value  = value;
    nouveau->next = NULL;

    if (use_mutex) pthread_mutex_lock(&l->mutex);

    if (l->head == NULL) {
        l->head = nouveau;
    } else {
        Node *courant = l->head;
        while (courant->next != NULL)
            courant = courant->next;
        courant->next = nouveau;
    }
    l->tail++;

    if (use_mutex) pthread_mutex_unlock(&l->mutex);
}

/* Retourne la tail courante (protégée) */
int list_size(Liste *l)
{
    if (use_mutex) pthread_mutex_lock(&l->mutex);
    int t = l->tail;
    if (use_mutex) pthread_mutex_unlock(&l->mutex);
    return t;
}

/* Retourne le noeud à l'index donné (protégé) */
int liste_get(Liste *l, int index)
{
    if (use_mutex) pthread_mutex_lock(&l->mutex);
    Node *courant = l->head;
    for (int i = 0; i < index && courant != NULL; i++)
        courant = courant->next;
    int val = courant ? courant->value : -1;
    if (use_mutex) pthread_mutex_unlock(&l->mutex);
    return val;
}

void list_destroy(Liste *l)
{
    Node *courant = l->head;
    while (courant) {
        Node *tmp = courant->next;
        free(courant);
        courant = tmp;
    }
    pthread_mutex_destroy(&l->mutex);
    free(l);
}

/* ── arguments passés aux threads ─────────────────────────────────── */
typedef struct {
    Liste *liste;
} Args;

/* ── thread producteur ────────────────────────────────────────────── */
void *producteur(void *arg)
{
    Liste *liste = ((Args *)arg)->liste;
    FILE  *urandom = fopen("/dev/urandom", "rb");
    if (!urandom) { perror("fopen /dev/urandom"); return NULL; }

    for (int i = 0; i < NB_AJOUTS; i++) {
        unsigned char octet;
        fread(&octet, 1, 1, urandom);

        /* Convertit en entier entre 0 et 255 */
        int value = (int)octet;

        list_add(liste, value);
        printf("[PROD] Ajout noeud %d : value=%d (tail=%d)\n",
               i + 1, value, list_size(liste));

        usleep(SLEEP_PROD_US);
    }

    fclose(urandom);
    printf("[PROD] Terminé.\n");
    return NULL;
}

/* ── thread consommateur ──────────────────────────────────────────── */
void *consommateur(void *arg)
{
    Liste *liste = ((Args *)arg)->liste;
    int    dernier_lu = 0; /* index du dernier noeud déjà affiché */

    /*
     * Le consommateur tourne jusqu'à avoir lu tous les noeuds.
     * Il poll la liste : sans pthread_cond, c'est du busy-waiting
     * allégé par usleep.
     */
    while (dernier_lu < NB_AJOUTS) {
        int tail = list_size(liste);

        /* Affiche tous les nouveaux noeuds depuis le dernier passage */
        while (dernier_lu < tail) {
            int val = liste_get(liste, dernier_lu);
            printf("  [CONS] Noeud %d lu : value=%d\n", dernier_lu + 1, val);
            dernier_lu++;
        }

        usleep(SLEEP_CONS_US);
    }

    printf("  [CONS] Terminé. %d noeuds lus.\n", dernier_lu);
    return NULL;
}

/* ── main ─────────────────────────────────────────────────────────── */
int main(int argc, char *argv[])
{
    if (argc >= 2 && strcmp(argv[1], "--no-mutex") == 0) {
        use_mutex = 0;
        printf("=== linkedLists.c (SANS mutex — mode cassé) ===\n\n");
        printf("ATTENTION : sans mutex, l'accès concurrent à la liste\n");
        printf("peut corrompre les pointeurs → crash ou données invalides.\n\n");
    } else {
        printf("=== linkedLists.c (AVEC mutex) ===\n\n");
    }

    /* Crée une liste avec un noeud de tête initial (value = 0) */
    Liste *liste = list_create();
    list_add(liste, 0);
    printf("[MAIN] Liste initialisée avec noeud de tête (value=0)\n\n");

    Args args = { .liste = liste };

    pthread_t t_prod, t_cons;
    pthread_create(&t_prod, NULL, producteur,    &args);
    pthread_create(&t_cons, NULL, consommateur,  &args);

    pthread_join(t_prod, NULL);
    pthread_join(t_cons, NULL);

    printf("\n[MAIN] Liste finale (%d noeuds) :\n", list_size(liste));
    for (int i = 0; i < list_size(liste); i++)
        printf("  [%d] = %d\n", i, liste_get(liste, i));

    list_destroy(liste);
    return 0;
}
