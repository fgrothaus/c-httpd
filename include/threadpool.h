#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>

#define QUEUE_SIZE 256
#define NUM_THREADS 8

typedef struct {
    int buffer[QUEUE_SIZE];
    int head;
    int tail;
    int size;
    pthread_mutex_t lock; // Mutex zum Locken
    pthread_cond_t not_empty; // Mutex sperrt Thread
    pthread_cond_t not_full; // Weckt Mainthread, sobald Platz in der Queue ist
} TaskQueue;

int queue_init(TaskQueue *queue);
int queue_push(TaskQueue *queue, int client_fd);
int queue_pop(TaskQueue *queue);

#endif