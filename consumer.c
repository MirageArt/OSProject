#include "buffer.h"
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

extern Buffer buffer;

void* consumer(void* arg) {
    int id = *(int*)arg;
    while (1) {
        sem_wait(&buffer.full);
        pthread_mutex_lock(&buffer.mutex);

        int item = remove_item(&buffer);
        printf("[Consumer-%d] Consumed: %d\n", id, item);

        pthread_mutex_unlock(&buffer.mutex);
        sem_post(&buffer.empty);

        usleep(150000);  // 150ms delay
    }
    return NULL;
}
