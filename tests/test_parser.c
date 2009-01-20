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

void testParser(void **state)
{
    queue_t *queue;
    command_t *cmd;

    initializeAliases();

    queue = parseInput("ls");
    assert_int_equal(queue->count, 1);
    cmd = queuePop(queue);
    assert_string_equal(cmd->path, "ls");
    assert_int_equal(cmd->connectionMask, commandConnectionNone);

    commandFree(cmd);
    queueFree(queue);

    queue = parseInput("ps aux         ");
    assert_int_equal(queue->count, 1);
    cmd = queuePop(queue);
    assert_string_equal(cmd->argv[0], "ps");
    assert_string_equal(cmd->argv[1], "aux");

    commandFree(cmd);
    queueFree(queue);

    queue = parseInput("            ps            aux         ");
    assert_int_equal(queue->count, 1);
    cmd = queuePop(queue);
    assert_string_equal(cmd->argv[0], "ps");
    assert_string_equal(cmd->argv[1], "aux");

    commandFree(cmd);
    queueFree(queue);

    queue = parseInput("ls -lh /");
    assert_int_equal(queue->count, 1);
    cmd = queuePop(queue);
    assert_string_equal(cmd->argv[0], "ls");
    assert_string_equal(cmd->argv[1], "-lh");
    assert_string_equal(cmd->argv[2], "/");

    commandFree(cmd);
    queueFree(queue);

    queue = parseInput("  ls           -lh       /        ");
    assert_int_equal(queue->count, 1);
    cmd = queuePop(queue);
    assert_string_equal(cmd->argv[0], "ls");
    assert_string_equal(cmd->argv[1], "-lh");
    assert_string_equal(cmd->argv[2], "/");

    commandFree(cmd);
    queueFree(queue);

    queue = parseInput(";");
    assert_int_equal(queue->count, 0);

    queueFree(queue);

    queue = parseInput("dmesg|tail");
    assert_int_equal(queue->count, 2);
    cmd = queuePop(queue);
    assert_string_equal(cmd->path, "dmesg");
    assert_int_equal(cmd->connectionMask, commandConnectionPipe);

    commandFree(cmd);
    queueFree(queue);

    queue = parseInput("dmesg | cat | cat | grep ACPI | wc -l");
    assert_int_equal(queue->count, 5);
    cmd = queuePop(queue);
    assert_string_equal(cmd->path, "dmesg");
    assert_int_equal(cmd->connectionMask, commandConnectionPipe);

    commandFree(cmd);
    queueFree(queue);

    queue = parseInput("ls; ls; ls; ls; ls");
    assert_int_equal(queue->count, 5);
    cmd = queuePop(queue);
    assert_string_equal(cmd->path, "ls");
    assert_int_equal(cmd->connectionMask, commandConnectionSequential);

    commandFree(cmd);
    queueFree(queue);

    queue = parseInput("");
    assert_int_equal(queue->count, 0);

    queueFree(queue);

    queue = parseInput("       ");
    assert_int_equal(queue->count, 0);

    queueFree(queue);

    queue = parseInput("cat > out  ");
    assert_int_equal(queue->count, 1);
    cmd = queuePop(queue);
    assert_true(cmd->redirectFromPath == NULL);
    assert_true(cmd->redirectToPath != NULL);
    assert_string_equal(cmd->redirectToPath, "out");
    assert_int_equal(cmd->connectionMask, commandConnectionNone);

    commandFree(cmd);
    queueFree(queue);

    queue = parseInput("cat <in   ");
    assert_int_equal(queue->count, 1);
    cmd = queuePop(queue);
    assert_true(cmd->redirectFromPath != NULL);
    assert_true(cmd->redirectToPath == NULL);
    assert_string_equal(cmd->redirectFromPath, "in");
    assert_int_equal(cmd->connectionMask, commandConnectionNone);

    commandFree(cmd);
    queueFree(queue);

    queue = parseInput("cat < in > out");
    assert_int_equal(queue->count, 1);
    cmd = queuePop(queue);
    assert_string_equal(cmd->path, "cat");
    assert_true(cmd->redirectToPath != NULL);
    assert_string_equal(cmd->redirectToPath, "out");
    assert_true(cmd->redirectFromPath != NULL);
    assert_string_equal(cmd->redirectFromPath, "in");
    assert_int_equal(cmd->connectionMask, commandConnectionNone);

    commandFree(cmd);
    queueFree(queue);

    queue = parseInput("cat > out < in");
    assert_int_equal(queue->count, 1);
    cmd = queuePop(queue);
    assert_true(cmd->redirectFromPath != NULL);
    assert_string_equal(cmd->redirectFromPath, "in");
    assert_true(cmd->redirectToPath != NULL);
    assert_string_equal(cmd->redirectToPath, "out");
    assert_int_equal(cmd->connectionMask, commandConnectionNone);

    commandFree(cmd);
    queueFree(queue);

    queue = parseInput("cat > out < in > out2");
    assert_int_equal(queue->count, 0);

    queueFree(queue);

    queue = parseInput("ps>out;dmesg|tail>out2");
    assert_int_equal(queue->count, 3);
    cmd = queuePop(queue);
    assert_true(cmd->redirectFromPath == NULL);
    assert_true(cmd->redirectToPath != NULL);
    assert_string_equal(cmd->redirectToPath, "out");
    commandFree(cmd);
    cmd = queuePop(queue);
    assert_true(cmd->redirectToPath == NULL);
    assert_true(cmd->redirectFromPath == NULL);
    commandFree(cmd);
    cmd = queuePop(queue);
    assert_true(cmd->redirectFromPath == NULL);
    assert_true(cmd->redirectToPath != NULL);
    assert_string_equal(cmd->redirectToPath, "out2");

    commandFree(cmd);
    queueFree(queue);

    queue = parseInput("cat > 'my output'");
    assert_int_equal(queue->count, 1);
    cmd = queuePop(queue);
    assert_true(cmd->redirectFromPath == NULL);
    assert_true(cmd->redirectToPath != NULL);
    assert_string_equal(cmd->redirectToPath, "my output");
    assert_int_equal(cmd->connectionMask, commandConnectionNone);

    commandFree(cmd);
    queueFree(queue);

    queue = parseInput("ps &");
    assert_int_equal(queue->count, 1);
    cmd = queuePop(queue);
    assert_true(cmd->redirectFromPath == NULL);
    assert_true(cmd->redirectToPath == NULL);
    assert_int_equal(cmd->connectionMask, commandConnectionBackground);

    commandFree(cmd);
    queueFree(queue);

    queue = parseInput("ls; dmesg|tail|wc -l; uname -a");
    assert_int_equal(queue->count, 5);
    cmd = queuePop(queue);
    assert_true(cmd->redirectFromPath == NULL);
    assert_true(cmd->redirectToPath == NULL);
    assert_int_equal(cmd->connectionMask, commandConnectionSequential);
    commandFree(cmd);
    cmd = queuePop(queue);
    assert_true(cmd->redirectFromPath == NULL);
    assert_true(cmd->redirectToPath == NULL);
    assert_int_equal(cmd->connectionMask, commandConnectionPipe);
    commandFree(cmd);
    cmd = queuePop(queue);
    assert_true(cmd->redirectFromPath == NULL);
    assert_true(cmd->redirectToPath == NULL);
    assert_int_equal(cmd->connectionMask, commandConnectionPipe);
    commandFree(cmd);
    cmd = queuePop(queue);
    assert_true(cmd->redirectFromPath == NULL);
    assert_true(cmd->redirectToPath == NULL);
    assert_int_equal(cmd->connectionMask, commandConnectionSequential);
    commandFree(cmd);
    cmd = queuePop(queue);
    assert_true(cmd->redirectFromPath == NULL);
    assert_true(cmd->redirectToPath == NULL);
    assert_int_equal(cmd->connectionMask, commandConnectionNone);

    commandFree(cmd);
    queueFree(queue);

    destroyAliases();
}

// vim: et ts=4 sw=4 sts=4
