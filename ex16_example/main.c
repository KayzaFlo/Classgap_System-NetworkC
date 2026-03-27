#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define TAILLE 5

int buffer[TAILLE];
int in = 0, out = 0, count = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  non_plein  = PTHREAD_COND_INITIALIZER;
pthread_cond_t  non_vide   = PTHREAD_COND_INITIALIZER;

void *producteur(void *arg) {
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&mutex);

        while (count == TAILLE)              // buffer plein → attendre
            pthread_cond_wait(&non_plein, &mutex);

        buffer[in] = i;
        in = (in + 1) % TAILLE;
        count++;
        printf("Produit : %d\n", i);

        pthread_cond_signal(&non_vide);      // réveille le consommateur
        pthread_mutex_unlock(&mutex);
        usleep(100000);
    }
    return NULL;
}

void *consommateur(void *arg) {
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&mutex);

        while (count == 0)                   // buffer vide → attendre
            pthread_cond_wait(&non_vide, &mutex);

        int val = buffer[out];
        out = (out + 1) % TAILLE;
        count--;
        printf("  Consommé : %d\n", val);

        pthread_cond_signal(&non_plein);     // réveille le producteur
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t tp, tc;
    pthread_create(&tp, NULL, producteur, NULL);
    pthread_create(&tc, NULL, consommateur, NULL);
    pthread_join(tp, NULL);
    pthread_join(tc, NULL);
    return 0;
}
