#include <stdio.h>
#include <stdlib.h>

#include "../include/threadpool.h"

int queue_init(TaskQueue *queue) {
    
    if (queue == NULL) return -1;
    
    queue->head = 0;
    queue->tail = 0;
    queue->size = 0;
    
    pthread_mutex_init(&queue->lock, NULL);
    pthread_cond_init(&queue->not_empty, NULL);
    pthread_cond_init(&queue->not_full, NULL);


    for (int i = 0; i < QUEUE_SIZE; i++) {
        queue->buffer[i] = 0;
    }

    return 0;
}

int queue_push(TaskQueue *queue, int client_fd) {

    if (queue == NULL) return -1;

    

    return 0;
}