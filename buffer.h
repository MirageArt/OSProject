#ifndef BUFFER_H
#define BUFFER_H

#include <semaphore.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    int value;
    int priority; // 0-4 range, -1 means empty slot
} BufferItem;

typedef struct {
    BufferItem* data;
    int size;
    int in;
    int out;
    sem_t empty;
    sem_t full;
    pthread_mutex_t mutex;
} Buffer;

void init_buffer(Buffer* buffer, int size);
void insert_item(Buffer* buffer, int item, int priority);
BufferItem remove_item(Buffer* buffer);
void destroy_buffer(Buffer* buffer);
void log_message(const char* thread_type, int id, const char* action, int value, int priority);

#endif
