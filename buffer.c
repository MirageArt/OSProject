#include "buffer.h"
#include <stdlib.h>
#include <stdio.h>

// Initialize buffer and synchronization primitives
void init_buffer(Buffer *buffer, int size) {
    buffer->data = (int *)malloc(size * sizeof(int));
    buffer->size = size;
    buffer->in = 0;
    buffer->out = 0;

    // Semaphores: 
    // - 'empty' starts with all slots available (count = size)
    // - 'full' starts with 0 filled slots
    sem_init(&buffer->empty, 0, size);
    sem_init(&buffer->full, 0, 0);

    // Mutex for thread-safe access
    pthread_mutex_init(&buffer->mutex, NULL);
}

// Insert item into buffer (producer)
void insert_item(Buffer *buffer, int item) {
    buffer->data[buffer->in] = item;
    buffer->in = (buffer->in + 1) % buffer->size; // Circular buffer
}

// Remove item from buffer (consumer)
int remove_item(Buffer *buffer) {
    int item = buffer->data[buffer->out];
    buffer->out = (buffer->out + 1) % buffer->size; // Circular buffer
    return item;
}

// Release resources
void destroy_buffer(Buffer *buffer) {
    free(buffer->data);
    sem_destroy(&buffer->empty);
    sem_destroy(&buffer->full);
    pthread_mutex_destroy(&buffer->mutex);
}
