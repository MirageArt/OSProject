#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

extern Buffer buffer;
#define NUM_ITEMS 20

// Thread-local seed (C11 or GCC/Clang)
#if defined(_MSC_VER)  // Windows (MSVC)
__declspec(thread) unsigned int seed;
#else                 // Linux/Mac (GCC/Clang)
__thread unsigned int seed;
#endif

void* producer(void* arg) {
    int id = *(int*)arg;

    // Initialize thread-local seed
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    seed = (unsigned int)(ts.tv_nsec ^ (unsigned long)pthread_self());
    srand(seed); // Seed rand() for this thread

    for (int i = 0; i < NUM_ITEMS; i++) {
        int item = rand() % 100;    // Random item 0–99
        int priority = rand() % 5;  // Priority 0–4

        sem_wait(&buffer.empty);
        pthread_mutex_lock(&buffer.mutex);

        insert_item(&buffer, item, priority);
        log_message("Producer", id, "Produced item", item, priority);
        printf("Buffer state: in=%d, out=%d\n", buffer.in, buffer.out);

        pthread_mutex_unlock(&buffer.mutex);
        sem_post(&buffer.full);

        usleep(100000 + (rand() % 50000)); // Delay 100–150ms
    }

    log_message("Producer", id, "Finished producing", NUM_ITEMS, -1);
    return NULL;
}
