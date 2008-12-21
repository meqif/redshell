/* See LICENSE file for copyright and license details. */

#ifndef QUEUE_H
#define QUEUE_H

typedef void (*queueNodeFreeFunction)(void *);

typedef struct __queue_node_t {
    void *data;
    struct __queue_node_t *next;
    queueNodeFreeFunction freeFunction;
} queue_node_t;

typedef struct __queue_t {
    size_t count;
    queue_node_t *head;
    queue_node_t *tail;
} queue_t;

queue_t *queueNew();
void queueInsert(queue_t *queue, void *data, queueNodeFreeFunction freeNode);
void *queuePop(queue_t *queue);
void queueFree(queue_t *queue);

#endif /* QUEUE_H */
