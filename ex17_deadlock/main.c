#include <pthread.h>
#include <stdio.h>

pthread_mutex_t A = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t B = PTHREAD_MUTEX_INITIALIZER;

void *thread1(void *arg) {
    pthread_mutex_lock(&A);   // prend A
    sleep(1);
    pthread_mutex_lock(&B);   // attend B → BLOQUÉ (T2 le tient)
    pthread_mutex_unlock(&B);
    pthread_mutex_unlock(&A);
    return NULL;
}

void *thread2(void *arg) {
    pthread_mutex_lock(&B);   // prend B
    sleep(1);
    pthread_mutex_lock(&A);   // attend A → BLOQUÉ (T1 le tient)
    pthread_mutex_unlock(&A);
    pthread_mutex_unlock(&B);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread1, NULL);
    pthread_create(&t2, NULL, thread2, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}
