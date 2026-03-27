#include <pthread.h>
#include <stdio.h>

void *ma_fonction(void *arg) {
    int id = *(int *)arg;
    printf("Thread %d en cours\n", id);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    int id1 = 1, id2 = 2;

    pthread_create(&t1, NULL, ma_fonction, &id1);
    pthread_create(&t2, NULL, ma_fonction, &id2);

    pthread_join(t1, NULL); // attend la fin de t1
    pthread_join(t2, NULL); // attend la fin de t2

    return 0;
}
