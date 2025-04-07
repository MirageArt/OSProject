#include "buffer.h"
#include "producer.h"
#include "consumer.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

Buffer buffer;
volatile sig_atomic_t stop = 0;

void handle_signal(int sig) {
    stop = 1;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s <num_producers> <num_consumers> <buffer_size>\n", argv[0]);
        return 1;
    }

    signal(SIGINT, handle_signal);

    // Seed random number generator once using high-precision clock
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    srand((unsigned)ts.tv_nsec);

    int num_producers = atoi(argv[1]);
    int num_consumers = atoi(argv[2]);
    int buffer_size = atoi(argv[3]);

    init_buffer(&buffer, buffer_size);

    pthread_t producers[num_producers];
    pthread_t consumers[num_consumers];
    int producer_ids[num_producers];
    int consumer_ids[num_consumers];

    // Create producers
    for (int i = 0; i < num_producers; i++) {
        producer_ids[i] = i + 1;
        pthread_create(&producers[i], NULL, producer, &producer_ids[i]);
    }

    // Create consumers
    for (int i = 0; i < num_consumers; i++) {
        consumer_ids[i] = i + 1;
        pthread_create(&consumers[i], NULL, consumer, &consumer_ids[i]);
    }

    // Wait for producers
    for (int i = 0; i < num_producers; i++) {
        pthread_join(producers[i], NULL);
    }

    // Wait for consumers to finish remaining items
    while (!stop) {
        int val;
        sem_getvalue(&buffer.full, &val);
        if (val == 0) break;
        usleep(100000);
    }

    // Terminate consumers
    for (int i = 0; i < num_consumers; i++) {
        pthread_cancel(consumers[i]);
        pthread_join(consumers[i], NULL);
        log_message("System", 0, "Terminated consumer", i + 1, -1);
    }

    destroy_buffer(&buffer);
    log_message("System", 0, "All threads terminated", 0, -1);
    return 0;
}
