/* See LICENSE file for copyright and license details. */

#include <assert.h>

#include "common.h"
#include "queue.h"

queue_t *queueNew(void)
{
    queue_t *queue = malloc(sizeof(*queue));
    if (queue != NULL) {
        queue->count = 0;
        queue->head  = NULL;
        queue->tail  = NULL;
    }
    return queue;
}

void queueInsert(queue_t *queue, void *data, queueNodeFreeFunction freeNode)
{
    queue_node_t *node;

    assert(queue != NULL);

    node = malloc(sizeof(*node));
    node->next = NULL;
    node->data = data;
    node->freeFunction = freeNode;

    if (queue->tail == NULL) {
        queue->tail = node;
        queue->head = node;
    } else {
        queue->tail->next = node;
        queue->tail = node;
    }
    queue->count++;
}

void *queuePop(queue_t *queue) {
    void *data;
    queue_node_t *next;

    if (queue->count == 0)
        return NULL;

    data = queue->head->data;
    next = queue->head->next;
    free(queue->head);
    queue->head = next;
    queue->count--;
    if (queue->head == NULL)
        queue->tail = NULL;
    return data;
}

void queueFree(queue_t *queue)
{
    while(queue->count > 0) {
        queue_node_t *node = queue->head;
        queue_node_t *next = queue->head->next;
        queueNodeFreeFunction freeNode = node->freeFunction;
        freeNode(node->data);
        free(node);
        queue->head = next;
        queue->count--;
    }
    free(queue);
}

// vim: et ts=4 sw=4 sts=4
