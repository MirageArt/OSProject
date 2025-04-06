#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_ITEMS_PER_PRODUCER 20

Buffer buffer;

void* producer(void* arg) {
    int producer_id = *(int*)arg;
    for (int i = 0; i < NUM_ITEMS_PER_PRODUCER; i++) {
        int item = rand() % 100;  // Generate random data

        sem_wait(&buffer.empty);  // Wait for empty slot
        pthread_mutex_lock(&buffer.mutex);

        insert_item(&buffer, item);
        printf("[Producer-%d] Produced item: %d\n", producer_id, item);

        pthread_mutex_unlock(&buffer.mutex);
        sem_post(&buffer.full);  // Signal new item

        usleep(100000);  // Simulate work (100ms)
    }
    printf("[Producer-%d] Finished producing %d items.\n", producer_id, NUM_ITEMS_PER_PRODUCER);
    return NULL;
}

void* consumer(void* arg) {
    int consumer_id = *(int*)arg;
    while (1) {
        sem_wait(&buffer.full);  // Wait for filled slot
        pthread_mutex_lock(&buffer.mutex);

        int item = remove_item(&buffer);
        printf("[Consumer-%d] Consumed item: %d\n", consumer_id, item);

        pthread_mutex_unlock(&buffer.mutex);
        sem_post(&buffer.empty);  // Signal free slot

        usleep(150000);  // Simulate work (150ms)
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s <num_producers> <num_consumers> <buffer_size>\n", argv[0]);
        return 1;
    }

    int num_producers = atoi(argv[1]);
    int num_consumers = atoi(argv[2]);
    int buffer_size = atoi(argv[3]);

    // Initialize buffer
    init_buffer(&buffer, buffer_size);

    pthread_t producers[num_producers];
    pthread_t consumers[num_consumers];
    int producer_ids[num_producers];
    int consumer_ids[num_consumers];

    // Create producer threads
    for (int i = 0; i < num_producers; i++) {
        producer_ids[i] = i + 1;
        pthread_create(&producers[i], NULL, producer, &producer_ids[i]);
    }

    // Create consumer threads
    for (int i = 0; i < num_consumers; i++) {
        consumer_ids[i] = i + 1;
        pthread_create(&consumers[i], NULL, consumer, &consumer_ids[i]);
    }

    // Wait for producers to finish
    for (int i = 0; i < num_producers; i++) {
        pthread_join(producers[i], NULL);
    }

    // Kill consumers after all items are produced (simplified)
    sleep(1);  // Allow consumers to finish processing
    for (int i = 0; i < num_consumers; i++) {
        pthread_cancel(consumers[i]);  // Force-terminate (not ideal, but works for demo)
    }

    // Cleanup
    destroy_buffer(&buffer);
    return 0;
}
