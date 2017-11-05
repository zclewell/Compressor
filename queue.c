/**
 * Splendid Synchronization Lab
 * CS 241 - Fall 2017
 */
#include "queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * This queue is implemented with a linked list of queue_nodes.
 */

typedef struct queue_node {
    void *data;
    struct queue_node *next;
} queue_node;

struct queue {
    /* queue_node pointers to the head and tail of the queue */
    queue_node *head, *tail;

    /* The number of elements in the queue */
    ssize_t size;

    /**
     * The maximum number of elements the queue can hold.
     * max_size is non-positive if the queue does not have a max size.
     */
    ssize_t max_size;

    /* Mutex and Condition Variable for thread-safety */
    pthread_cond_t cv;
    pthread_mutex_t m;
};

queue *queue_create(ssize_t max_size) {
    queue *q = malloc(sizeof(queue));
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    q->max_size = max_size;
    pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cv = PTHREAD_COND_INITIALIZER; 
    pthread_mutex_init(&m,NULL);
    pthread_cond_init(&cv,NULL);
    q->m = m;
    q->cv = cv;
    return q;
    /* Your code here */
}

void queue_destroy(queue *this) {
    pthread_mutex_t *m = &(this->m);
    pthread_mutex_destroy(m);
    pthread_cond_t *cv = &(this->cv);
    pthread_cond_destroy(cv);
    /* Your code here */
}

void queue_push(queue *this, void *data) {
    // printf("Entering push...\n");
    pthread_mutex_t *m = &(this->m);
    pthread_cond_t *cv = &(this->cv);
    pthread_mutex_lock(m);
    // printf("Got the lock...\n");
    if (this->max_size > 0) {
        while( this->size == this->max_size) {
            pthread_cond_wait(cv,m);
        }
    }
    queue_node *new_node = malloc(sizeof(queue_node));
    // printf("Got here\n");
    if (!new_node) {
        exit(1);
    }
    new_node->data = data;
    new_node->next = NULL;
    if (this->tail == NULL) {
        // printf("No head found, creating one...\n");
        this->tail = new_node;
        this->head = new_node;
    } else {
        // printf("Adding to end of list\n");
        this->tail->next = new_node;
        this->tail = new_node;
    }
    this->size = this->size + 1;    
    // fprintf(stdout, "Size is now: %zd\n",this->size );

    pthread_cond_broadcast(cv);
    pthread_mutex_unlock(m);
    // printf("Exiting push...\n");
}

void *queue_pull(queue *this) {
    pthread_mutex_t *m = &(this->m);
    pthread_cond_t *cv = &(this->cv);
    pthread_mutex_lock(m);
    while( this->size == 0 ) {
        pthread_cond_wait(cv,m);
    } 
    this->size = this->size - 1;
    // printf("Size ok\n");
    queue_node* output = this->head;
    if (this->head) {
        if (this->head->next) {
            this->head = this->head->next;
        } else {
            this->head = NULL;
            this->tail = NULL;
        }
    }
    // fprintf(stdout, "Size is now: %zd\n",this->size );
    pthread_cond_broadcast(cv);
    pthread_mutex_unlock(m);
    // printf("Exiting pull...\n");
    return output->data;
}
