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

    queue = interpret_line("ls");
    assert_int_equal(queue->count, 1);
    cmd = queuePop(queue);
    assert_string_equal(cmd->path, "ls");
    assert_int_equal(cmd->connectionMask, commandConnectionNone);

    commandFree(cmd);
    queueFree(queue);

    queue = interpret_line("ps aux         ");
    assert_int_equal(queue->count, 1);
    cmd = queuePop(queue);
    assert_string_equal(cmd->argv[0], "ps");
    assert_string_equal(cmd->argv[1], "aux");

    commandFree(cmd);
    queueFree(queue);

    queue = interpret_line("            ps            aux         ");
    assert_int_equal(queue->count, 1);
    cmd = queuePop(queue);
    assert_string_equal(cmd->argv[0], "ps");
    assert_string_equal(cmd->argv[1], "aux");

    commandFree(cmd);
    queueFree(queue);

    queue = interpret_line("ls -lh /");
    assert_int_equal(queue->count, 1);
    cmd = queuePop(queue);
    assert_string_equal(cmd->argv[0], "ls");
    assert_string_equal(cmd->argv[1], "-lh");
    assert_string_equal(cmd->argv[2], "/");

    commandFree(cmd);
    queueFree(queue);

    queue = interpret_line("  ls           -lh       /        ");
    assert_int_equal(queue->count, 1);
    cmd = queuePop(queue);
    assert_string_equal(cmd->argv[0], "ls");
    assert_string_equal(cmd->argv[1], "-lh");
    assert_string_equal(cmd->argv[2], "/");

    commandFree(cmd);
    queueFree(queue);

    queue = interpret_line(";");
    assert_int_equal(queue->count, 0);

    queueFree(queue);

    queue = interpret_line("dmesg|tail");
    assert_int_equal(queue->count, 2);
    cmd = queuePop(queue);
    assert_string_equal(cmd->path, "dmesg");
    assert_int_equal(cmd->connectionMask, commandConnectionPipe);

    commandFree(cmd);
    queueFree(queue);

    queue = interpret_line("ls; ls; ls; ls; ls");
    assert_int_equal(queue->count, 5);
    cmd = queuePop(queue);
    assert_string_equal(cmd->path, "ls");
    assert_int_equal(cmd->connectionMask, commandConnectionSequential);

    commandFree(cmd);
    queueFree(queue);

    queue = interpret_line("");
    assert_int_equal(queue->count, 0);

    queueFree(queue);

    queue = interpret_line("");
    assert_int_equal(queue->count, 0);

    queueFree(queue);

    queue = interpret_line("       ");
    assert_int_equal(queue->count, 0);

    queueFree(queue);

    queue = interpret_line("cat > out  ");
    assert_int_equal(queue->count, 1);
    cmd = queuePop(queue);
    assert_true(cmd->redirectFromPath == NULL);
    assert_true(cmd->redirectToPath != NULL);
    assert_string_equal(cmd->redirectToPath, "out");
    assert_int_equal(cmd->connectionMask, commandConnectionNone);

    commandFree(cmd);
    queueFree(queue);

    queue = interpret_line("cat <in   ");
    assert_int_equal(queue->count, 1);
    cmd = queuePop(queue);
    assert_true(cmd->redirectFromPath != NULL);
    assert_true(cmd->redirectToPath == NULL);
    assert_string_equal(cmd->redirectFromPath, "in");
    assert_int_equal(cmd->connectionMask, commandConnectionNone);

    commandFree(cmd);
    queueFree(queue);

    queue = interpret_line("cat < in > out");
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

    queue = interpret_line("cat > out < in");
    assert_int_equal(queue->count, 1);
    cmd = queuePop(queue);
    assert_true(cmd->redirectFromPath != NULL);
    assert_string_equal(cmd->redirectFromPath, "in");
    assert_true(cmd->redirectToPath != NULL);
    assert_string_equal(cmd->redirectToPath, "out");
    assert_int_equal(cmd->connectionMask, commandConnectionNone);

    commandFree(cmd);
    queueFree(queue);

    queue = interpret_line("cat > out < in > out2");
    assert_int_equal(queue->count, 0);

    queueFree(queue);

    queue = interpret_line("ps>out;dmesg|tail>out2");
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

    queue = interpret_line("cat > 'my output'");
    assert_int_equal(queue->count, 1);
    cmd = queuePop(queue);
    assert_true(cmd->redirectFromPath == NULL);
    assert_true(cmd->redirectToPath != NULL);
    assert_string_equal(cmd->redirectToPath, "my output");
    assert_int_equal(cmd->connectionMask, commandConnectionNone);

    commandFree(cmd);
    queueFree(queue);

    queue = interpret_line("ps &");
    assert_int_equal(queue->count, 1);
    cmd = queuePop(queue);
    assert_true(cmd->redirectFromPath == NULL);
    assert_true(cmd->redirectToPath == NULL);
    assert_int_equal(cmd->connectionMask, commandConnectionBackground);

    commandFree(cmd);
    queueFree(queue);

    destroyAliases();
}

// vim: et ts=4 sw=4 sts=4
