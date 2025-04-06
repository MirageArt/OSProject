#ifndef BUFFER_H
#define BUFFER_H

#include <semaphore.h>
#include <pthread.h>

typedef struct {
    int *data;          // Buffer array (dynamic)
    int size;           // Max buffer capacity
    int in;             // Index for next insertion (producer)
    int out;            // Index for next removal (consumer)
    sem_t empty;        // Tracks empty slots
    sem_t full;         // Tracks filled slots
    pthread_mutex_t mutex; // Ensures mutual exclusion
} Buffer;

// Initialize buffer with given size
void init_buffer(Buffer *buffer, int size);

// Insert an item into the buffer (thread-safe)
void insert_item(Buffer *buffer, int item);

// Remove an item from the buffer (thread-safe)
int remove_item(Buffer *buffer);

// Clean up buffer resources (semaphores, mutex, memory)
void destroy_buffer(Buffer *buffer);

#endif
