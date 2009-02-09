/*
 * Copyright (c) 2008-2009 Ricardo Martins <ricardo@scarybox.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

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
