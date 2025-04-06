#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

extern Buffer buffer;
#define NUM_ITEMS 20

void* producer(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < NUM_ITEMS; i++) {
        int item = rand() % 100;

        sem_wait(&buffer.empty);
        pthread_mutex_lock(&buffer.mutex);

        insert_item(&buffer, item);
        printf("[Producer-%d] Produced: %d\n", id, item);

        pthread_mutex_unlock(&buffer.mutex);
        sem_post(&buffer.full);

        usleep(100000);  // 100ms delay
    }
    printf("[Producer-%d] Done.\n", id);
    return NULL;
}
