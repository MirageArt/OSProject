#include "buffer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void log_message(const char* thread_type, int id, const char* action, int value, int priority) {
    time_t now;
    time(&now);
    char timestamp[20];
    strftime(timestamp, 20, "%H:%M:%S", localtime(&now));

    printf("[%s] [%s-%d] %s: %d (Priority: %d)\n",
        timestamp, thread_type, id, action, value, priority);
}

void init_buffer(Buffer* buffer, int size) {
    buffer->data = (BufferItem*)calloc(size, sizeof(BufferItem));
    buffer->size = size;
    buffer->in = 0;
    buffer->out = 0;

    for (int i = 0; i < size; i++) {
        buffer->data[i].priority = -1; // Mark as empty
    }

    sem_init(&buffer->empty, 0, size);
    sem_init(&buffer->full, 0, 0);
    pthread_mutex_init(&buffer->mutex, NULL);
}

void insert_item(Buffer* buffer, int item, int priority) {
    // Ensure priority is 0-4
    priority = (priority < 0) ? 0 : (priority > 4) ? 4 : priority;

    buffer->data[buffer->in].value = item;
    buffer->data[buffer->in].priority = priority;
    buffer->in = (buffer->in + 1) % buffer->size;
}

BufferItem remove_item(Buffer* buffer) {
    // Find highest priority valid item
    int highest_priority = -1;
    int best_index = buffer->out;

    for (int i = 0; i < buffer->size; i++) {
        int idx = (buffer->out + i) % buffer->size;
        if (buffer->data[idx].priority > highest_priority) {
            highest_priority = buffer->data[idx].priority;
            best_index = idx;
        }
    }

    if (highest_priority == -1) {
        return (BufferItem) { -1, -1 }; // No valid items
    }

    // Swap if needed
    if (best_index != buffer->out) {
        BufferItem temp = buffer->data[buffer->out];
        buffer->data[buffer->out] = buffer->data[best_index];
        buffer->data[best_index] = temp;
    }

    BufferItem item = buffer->data[buffer->out];
    buffer->data[buffer->out].priority = -1; // Mark as empty
    buffer->out = (buffer->out + 1) % buffer->size;
    return item;
}

void destroy_buffer(Buffer* buffer) {
    free(buffer->data);
    sem_destroy(&buffer->empty);
    sem_destroy(&buffer->full);
    pthread_mutex_destroy(&buffer->mutex);
}
