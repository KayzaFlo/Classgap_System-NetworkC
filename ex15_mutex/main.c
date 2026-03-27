#include <pthread.h>
#include <stdio.h>

int counter = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *incrementer(void *arg) {
    for (int i = 0; i < 100000; i++) {
        // SANS mutex : résultat imprévisible
        // counter++;

        // AVEC mutex : résultat garanti
        pthread_mutex_lock(&mutex);
        counter++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, incrementer, NULL);
    pthread_create(&t2, NULL, incrementer, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("counter = %d (attendu : 200000)\n", counter);
    pthread_mutex_destroy(&mutex);
    return 0;
}
