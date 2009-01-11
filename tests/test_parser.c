/* See LICENSE file for copyright and license details. */

#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmockery.h>
#include <unistd.h>
#include <string.h>

#include "alias.h"
#include "common.h"
#include "parser.h"

void testRedirectionParser(void **state)
{
    char *line = NULL;
    char *paths[2];

    paths[0] = NULL;
    paths[1] = NULL;
    line = calloc(12, sizeof(char));
    strcpy(line, "cat > out  ");
    getRedirectionPaths(line, paths);
    assert_true(paths[0] == NULL);
    assert_true(paths[1] != NULL);
    if (paths[1] != NULL) {
        assert_string_equal(paths[1], "out");
        free(paths[1]);
    }
    free(line);

    paths[0] = NULL;
    paths[1] = NULL;
    line = calloc(11, sizeof(char));
    strcpy(line, "cat <in   ");
    getRedirectionPaths(line, paths);
    assert_true(paths[0] != NULL);
    assert_true(paths[1] == NULL);
    if (paths[0] != NULL) {
        assert_string_equal(paths[0], "in");
        free(paths[0]);
    }
    free(line);

    paths[0] = NULL;
    paths[1] = NULL;
    line = calloc(15, sizeof(char));
    strcpy(line, "cat > out < in");
    getRedirectionPaths(line, paths);
    assert_true(paths[0] != NULL);
    if (paths[0] != NULL) {
        assert_string_equal(paths[0], "in");
        free(paths[0]);
    }
    assert_true(paths[1] != NULL);
    if (paths[1] != NULL) {
        assert_string_equal(paths[1], "out");
        free(paths[1]);
    }
    free(line);
}

void testParser(void **state)
{
    queue_t *queue;
    command_t *cmd;

    initializeAliases();

    queue = interpret_line("ls");
    assert_int_equal(queue->count, 1);
    cmd = queuePop(queue);
    assert_string_equal(cmd->path, "ls");
    assert_int_equal(cmd->connectionMask, commandConnectionNone);

    commandFree(cmd);
    queueFree(queue);

    queue = interpret_line("dmesg|tail");
    assert_int_equal(queue->count, 2);
    cmd = queuePop(queue);
    assert_string_equal(cmd->path, "dmesg");
    assert_int_equal(cmd->connectionMask, commandConnectionPipe);


    queueFree(queue);

    queue = interpret_line("ls; ls; ls; ls; ls");
    assert_int_equal(queue->count, 5);
    cmd = queuePop(queue);
    assert_string_equal(cmd->path, "ls");
    assert_int_equal(cmd->connectionMask, commandConnectionSequential);

    queueFree(queue);

    queue = interpret_line("");
    assert_int_equal(queue->count, 0);

    queueFree(queue);

    queue = interpret_line("       ");
    assert_int_equal(queue->count, 0);

    queueFree(queue);

    char *line = malloc(15 * sizeof(char));
    strcpy(line, "cat < in > out");
    queue = interpret_line(line);
    assert_int_equal(queue->count, 1);
    cmd = queuePop(queue);
    assert_string_equal(cmd->path, "cat");
    assert_true(cmd->redirectToPath != NULL);
    if (cmd->redirectToPath != NULL)
        assert_string_equal(cmd->redirectToPath, "out");
    assert_true(cmd->redirectFromPath != NULL);
    if (cmd->redirectFromPath != NULL)
        assert_string_equal(cmd->redirectFromPath, "in");

    free(line);
    commandFree(cmd);
    queueFree(queue);

    destroyAliases();
}

// vim: et ts=4 sw=4 sts=4
