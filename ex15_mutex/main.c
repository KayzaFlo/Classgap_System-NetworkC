#include <pthread.h>
#include <stdio.h>

int counter = 0;
// pthread_mutex_t mutexA = PTHREAD_MUTEX_INITIALIZER;

void *incrementer(void *arg) {
    for (int i = 0; i < 100000; i++) {
        // SANS mutex : résultat imprévisible
        // counter++;
		
		pthread_mutex_t *mutexB = (pthread_mutex_t *)arg;

        // AVEC mutex : résultat garanti
        pthread_mutex_lock(mutexB);
        counter++;
        pthread_mutex_unlock(mutexB);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
	pthread_mutex_t mutexB;
	pthread_mutex_init(&mutexB, NULL); // Plus propre que PTHREAD_MUTEX_INITIALIZER pour un mutex local
    pthread_create(&t1, NULL, incrementer, &mutexB);
    pthread_create(&t2, NULL, incrementer, &mutexB);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("counter = %d (attendu : 200000)\n", counter);
    pthread_mutex_destroy(&mutexB);
    return 0;
}


// Th_A pthread_mutex_lock(&mutexA);
// Th_A lire counter = 100

// Th_B pthread_mutex_lock(&mutexA);

// Th_A calcul 100 + 1
// Th_A ecrit 101 dans counter
// Th_A pthread_mutex_unlock(&mutexA);

// Th_B pthread_mutex_lock(&mutexA);
// Th_B lire counter = 101
// Th_B calcul 101 + 1
// Th_B ecrit 102 dans counter




