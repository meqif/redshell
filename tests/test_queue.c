/* See LICENSE file for copyright and license details. */

#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmockery.h>
#include <unistd.h>

#include "common.h"
#include "queue.h"

static queue_t *queue;

void testCreateQueue(void **state)
{
    queue = queueNew();
    assert_false(queue == NULL);
}

void testInsertIntoQueue(void **state)
{
    char *original_data, *original_data2, *copy_data;

    original_data = calloc(5, sizeof(char));
    strcpy(original_data, "bleh");
    queueInsert(queue, original_data, free);
    assert_int_equal(queue->count, 1);
    copy_data = queuePop(queue);
    assert_string_equal(copy_data, original_data);
    assert_int_equal(queue->count, 0);
    free(copy_data);

    original_data = calloc(5, sizeof(char));
    strcpy(original_data, "bleh");
    queueInsert(queue, original_data, free);
    original_data2 = calloc(5, sizeof(char));
    strcpy(original_data2, "zomg");
    queueInsert(queue, original_data2, free);
    assert_int_equal(queue->count, 2);
    copy_data = queuePop(queue);
    assert_string_equal(copy_data, original_data);
    assert_int_equal(queue->count, 1);
    free(copy_data);
}

void testDestroyQueue(void **state)
{
    queueFree(queue);
}

// vim: et ts=4 sw=4 sts=4
