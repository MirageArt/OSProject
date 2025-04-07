#include "buffer.h"
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

extern Buffer buffer;

void* consumer(void* arg) {
    int id = *(int*)arg;
    int consumed_count = 0;

    while (1) {
        sem_wait(&buffer.full);
        pthread_mutex_lock(&buffer.mutex);

        BufferItem item = remove_item(&buffer);
        if (item.priority != -1) { // Only process valid items
            log_message("Consumer", id, "Consumed item", item.value, item.priority);
            printf("Buffer state: in=%d, out=%d\n", buffer.in, buffer.out);
            consumed_count++;
        }

        pthread_mutex_unlock(&buffer.mutex);
        sem_post(&buffer.empty);

        usleep(150000); // 150ms delay
    }
    return NULL;
}
