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

queue_t *queueNew(void);
void queueInsert(queue_t *queue, void *data, queueNodeFreeFunction freeNode);
void *queuePop(queue_t *queue);
void queueFree(queue_t *queue);

#endif /* QUEUE_H */
