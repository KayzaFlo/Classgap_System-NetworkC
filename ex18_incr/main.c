/*
 * Démontre la race condition sur un compteur partagé.
 * Compilez : gcc incr.c -o incr -lpthread
 * Usage    : ./incr <nb_threads> <nb_increments> [--mutex]
 *
 * Exemple :
 *   ./incr 4 100000          -> résultat imprévisible
 *   ./incr 4 100000 --mutex  -> toujours correct
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

/* ── variables globales partagées ─────────────────────────────────── */
long counter = 0;
int  use_mutex = 0;
long increments_per_thread = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* ── fonction exécutée par chaque thread ──────────────────────────── */
void *increment(void *arg)
{
    int tid = *(int *)arg;

    for (long i = 0; i < increments_per_thread; i++) {
        if (use_mutex) {
            pthread_mutex_lock(&mutex);
            counter++;
            pthread_mutex_unlock(&mutex);
        } else {
            /*
             * SANS mutex : cette ligne compile en 3 instructions CPU :
             *   1. lire  counter dans un registre
             *   2. ajouter 1
             *   3. écrire le résultat dans counter
             * Le scheduler peut interrompre le thread ENTRE ces étapes
             * → deux threads lisent la même valeur → une incrémentation
             *   est perdue (race condition).
             */
            counter++;
        }
    }

    printf("  Thread %d terminé\n", tid);
    return NULL;
}

/* ── main ─────────────────────────────────────────────────────────── */
int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <nb_threads> <nb_increments> [--mutex]\n", argv[0]);
        return 1;
    }

    int nb_threads = atoi(argv[1]);
    increments_per_thread = atol(argv[2]);

    if (argc >= 4 && strcmp(argv[3], "--mutex") == 0)
        use_mutex = 1;

    long attendu = (long)nb_threads * increments_per_thread;

    printf("=== incr.c ===\n");
    printf("Threads : %d | Incréments/thread : %ld | Attendu : %ld\n",
           nb_threads, increments_per_thread, attendu);
    printf("Mode : %s\n\n", use_mutex ? "AVEC mutex" : "SANS mutex (race condition)");

    /* Allocation des threads et de leurs IDs */
    pthread_t *threads = malloc(nb_threads * sizeof(pthread_t));
    int       *ids     = malloc(nb_threads * sizeof(int));

    /* Création des threads */
    for (int i = 0; i < nb_threads; i++) {
        ids[i] = i + 1;
        if (pthread_create(&threads[i], NULL, increment, &ids[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    /* Attendre la fin de tous les threads */
    for (int i = 0; i < nb_threads; i++)
        pthread_join(threads[i], NULL);

    printf("\nRésultat : counter = %ld\n", counter);
    printf("Attendu  : %ld\n", attendu);

    if (counter == attendu)
        printf("CORRECT\n");
    else
        printf("INCORRECT — %ld increments perdus (%.1f%%)\n",
               attendu - counter,
               100.0 * (attendu - counter) / attendu);

    pthread_mutex_destroy(&mutex);
    free(threads);
    free(ids);
    return 0;
}
