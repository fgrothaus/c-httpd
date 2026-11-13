#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/threadpool.h"

// Der Mutex wird immer nur für die Queue gesetzt.
// Mainthread legt neue Client FDs in die Queue und sperrt den Zugang zur Queue in der Zeit für die Subthreads
// Subthread nimmt einen FD aus der Queue und sperrt den Zugang zur Queue, während er den FD entnimmt. Anschließend rechnet er unabhängig weiter und liefert Response für den HTTP-Client

// Mainthread wartet auf Subthread, wenn Queue voll ist
// Subthread wartet auf Mainthread, wenn Queue leer ist

int queue_init(TaskQueue *queue) {
    
    if (queue == NULL) return -1;
    
    queue->head = 0;
    queue->tail = 0;
    queue->counter = 0;
    
    pthread_mutex_init(&queue->lock, NULL);
    pthread_cond_init(&queue->not_empty, NULL);
    pthread_cond_init(&queue->not_full, NULL);

    memset(queue->buffer, 0, sizeof(queue->buffer));

    return 0;
}

void queue_push(TaskQueue *queue, int client_fd) {

    pthread_mutex_lock(&queue->lock); // queue absperren, damit während des Pushes kein anderer Thread dazwischenfunkt

    // Solange Queue voll ist, wird gewartet, bis Subthread Queue wieder freigibt
    while (queue->counter == QUEUE_SIZE) {
        pthread_cond_wait(&queue->not_full, &queue->lock); // &queue->not_full sagt while in queue_pop, 
    }

    queue->buffer[queue->tail] = client_fd;
    queue->tail = (queue->tail + 1) % QUEUE_SIZE;
    queue->counter++;

    pthread_cond_signal(&queue->not_empty); // Mainthread sagt while in queue_pop, dass Subthread geweckt werden soll

    pthread_mutex_unlock(&queue->lock); // Mainthread gibt die Queue wieder frei
}

int queue_pop(TaskQueue *queue) {

    pthread_mutex_lock(&queue->lock);

    // Solange Queue leer ist, wird gewartet, bis Mainthread neuen Client FD in die Queue legt und die Queue wieder freigibt
    while (queue->counter == 0) {
        pthread_cond_wait(&queue->not_empty, &queue->lock); // &queue->lock = Queue wird für Mainthread wieder freigegeben und wartet auf Wecksignal
    }

    int client_fd = queue->buffer[queue->head];
    queue->head = (queue->head + 1) % QUEUE_SIZE;
    queue->counter--;

    pthread_cond_signal(&queue->not_full); // Subthread sagt while in queue_push, dass er fertig ist 

    pthread_mutex_unlock(&queue->lock);

    return client_fd;
}