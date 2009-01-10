/*
 * redshell - simple and light UNIX shell
 * jobs.c: job control
 *
 * Copyright (C) 2008 Ricardo Martins <ricardo@scarybox.net>
 * See LICENSE or http://www.opensource.org/licenses/mit-license.php
 * for copyright and license details.
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>

#include "alias.h"
#include "builtins.h"
#include "common.h"
#include "helper.h"
#include "jobs.h"

#define PERMS 0644

extern pid_t fg_pid;
extern pid_t *pids;

/* Add pid to the first empty index in pids array */
int add_pid(pid_t new_pid)
{
    int i;
    for (i = 0; i < HIST_SIZE; i++) {
        if (pids[i] == 0) {
            pids[i] = new_pid;
            return i;
        }
    }
    return -1;
}

/* Run given builtin command, if possible */
static int _executeBuiltinCommand(char **cmd)
{
    unsigned int i;
    for (i = 0; i < ARRAY_SIZE(commands); i++) {
        const struct cmd_struct *p = commands+i;
        if (strcmp(p->cmd, *cmd))
            continue;
        (p->fn)(cmd+1);
        return 0;
    }
    return -1;
}

/* Exec wrapper that performs some error handling */
int executeCommand(command_t *cmd)
{
    /* First, check if it is a builtin command */
    if (_executeBuiltinCommand(cmd->argv) == 0) return 0;

    /* If not, try to execute as an external command */
    int status = execvp(cmd->path, cmd->argv);
    if (status == -1) {
        if (errno == ENOENT)
            fprintf(stderr, "%s: command not found\n", cmd->path);
        else
            perror(cmd->path);
    }
    return status;
}

/* Closes all pipes */
static void _closepipes(int *pipes, int count)
{
    int i;
    for (i = 0; i < count; i++)
        close(pipes[i]);
}

/* Executes command queue */
int executeCommandsInQueue(queue_t *commandQueue)
{
    assert(commandQueue != NULL);

    if (commandQueue->count == 0)
        return 0;

    int i;
    int status;
    int n_commands = commandQueue->count;
    int tot_pipes = 2*(n_commands-1); /* Total pipe ends */
    int pipes[tot_pipes];
    int fd_in = -1;
    int fd_out = -1;
    int stdin_copy  = -1;
    int stdout_copy = -1;
    pid_t launched[n_commands];
    pid_t pid = -1;
    command_t *cmd = NULL;
    command_t *lastCommand = NULL;

    /* Save original stdin and stdout */
    stdin_copy  = dup(0);
    stdout_copy = dup(1);

    /* Initialize pipes */
    for (i = 0; i < tot_pipes; i += 2)
        pipe(pipes+i);

    for (i = 0; i < n_commands; i++) {
        if (lastCommand != NULL)
            commandFree(lastCommand);
        lastCommand = cmd;

        cmd = queuePop(commandQueue);

        /* Parte de um pipe */
        if ( (lastCommand != NULL && lastCommand->connectionMask == commandConnectionPipe) ||
                cmd->connectionMask == commandConnectionPipe) {

            /* Piped commands */
            pid = fork();
            if (pid == 0) {
                /* Redirect input */
                if (cmd->redirectFromPath != NULL) {
                    fd_in = open(cmd->redirectFromPath, O_RDONLY);
                    if (fd_in == -1) {
                        perror(cmd->redirectFromPath);
                        return -1;
                    }
                }

                /* Redirect output */
                if (cmd->redirectToPath != NULL) {
                    fd_out = open(cmd->redirectToPath, O_WRONLY|O_CREAT|O_TRUNC, PERMS);
                    if (fd_out == -1) {
                        perror(cmd->redirectToPath);
                        if (fd_in == -1) close(fd_in);
                        return -1;
                    }
                }

                if (i == 0) {               /* First command */
                    if (n_commands > 1) dup2(pipes[1], 1);
                    if (fd_in != -1)    dup2(fd_in, 0);
                }
                if (i == n_commands-1) {    /* Last command */
                    if (n_commands > 1) dup2(pipes[2*(i-1)], 0);
                    if (fd_out != -1)   dup2(fd_out, 1);
                }
                else {                      /* Everything in between */
                    dup2(pipes[2*(i-1)], 0);
                    dup2(pipes[(2*i)+1], 1);
                }
                _closepipes(pipes, tot_pipes);
                status = executeCommand(cmd);
                commandFree(cmd);
                queueFree(commandQueue);
                exit(status);
            }
            else
                fg_pid = launched[i] = pid;
        }
        else {

            if (lastCommand != NULL && pid != -1 &&
                    lastCommand->connectionMask != commandConnectionPipe &&
                    lastCommand->connectionMask != commandConnectionBackground) {
                fprintf(stderr, "Waiting for %s %d\n", lastCommand->path, pid);
                waitpid(pid, NULL, 0);
            }

            /* Single builtin command */
            /* This is needed for commands like 'exit' and 'cd' */
            if (cmd->connectionMask != commandConnectionBackground &&
                    commandQueue->count == 0 && isBuiltin(cmd->path)) {
                executeCommand(cmd);
                return 0;
            }

            /* Non-piped commands */
            if (cmd->connectionMask != commandConnectionPipe &&
                    (lastCommand == NULL || lastCommand->connectionMask != commandConnectionPipe)) {
                pid = fork();
                if (pid == 0) {
                    status = executeCommand(cmd);
                    exit(status);
                }
            }
        }
    }
    lastCommand = cmd;

    /* Only the parent gets here and waits for children to finish */
    _closepipes(pipes, tot_pipes);

    /* Restore stdin and stdout */
    dup2(stdin_copy,  0);
    dup2(stdout_copy, 1);

    if (lastCommand != NULL && pid != -1 &&
            lastCommand->connectionMask != commandConnectionPipe &&
            lastCommand->connectionMask != commandConnectionBackground)
        waitpid(pid, NULL, 0);

//    if (!commandQueue->bg)
        for (i = 0; i < n_commands; i++)
            while (waitpid(launched[i], NULL, 0) == -1 && errno != ECHILD);
//    else
//        for (i = 0; i < n_commands; i++) {
//            pid_counter = add_pid(launched[i]);
//            printf("[%d] %d\n", pid_counter+1, launched[i]);
//        }

    /* Since either the execution of the external command ended or it was
     * on the background, we have nothing running on the foreground. */
    fg_pid = 0;

    return 0;
}

// vim: et ts=4 sw=4 sts=4
