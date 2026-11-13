#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>

#define QUEUE_SIZE 256
#define NUM_THREADS 8

typedef struct {
    int buffer[QUEUE_SIZE];
    int head;
    int tail;
    int counter;
    pthread_mutex_t lock; // Mutex zum Locken
    pthread_cond_t not_empty; // Es gibt Arbeit - Klingel für Workerthreads
    pthread_cond_t not_full; // Es ist wieder Platz - Klingel für Mainthread
} TaskQueue;

int queue_init(TaskQueue *queue);
void queue_push(TaskQueue *queue, int client_fd);
int queue_pop(TaskQueue *queue);

#endif