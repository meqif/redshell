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
#include <fcntl.h>
#include <errno.h>

#include "alias.h"
#include "builtins.h"
#include "common.h"
#include "helper.h"

#define PERMS 0644

extern pid_t fg_pid;

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
    char *alias = getAlias(cmd->path);
    if (alias != NULL) {
        cmd->argv[0] = strdup(alias);
        cmd->path = cmd->argv[0];
    }
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
static void closepipes(int *pipes, int count)
{
    int i;
    for (i = 0; i < count; i++)
        close(pipes[i]);
}

/* Executes several external commands, with pipelines */
int spawnCommand(pipeline_t *pipeline)
{
    int i;
    int status;
    int pid_counter;
    int n_commands = pipeline->pipes;
    int tot_pipes = 2*(n_commands-1); /* Total pipe ends */
    int pipes[tot_pipes];
    int fd_in = -1;
    int fd_out = -1;
    int stdin_copy  = -1;
    int stdout_copy = -1;
    pid_t launched[n_commands];

    /* Save original stdin and stdout */
    stdin_copy  = dup(0);
    stdout_copy = dup(1);

    /* Redirect input */
    if (pipeline->redirectFromPath != NULL) {
        fd_in = open(pipeline->redirectFromPath, O_RDONLY);
        if (fd_in == -1) {
            perror(pipeline->redirectFromPath);
            return -1;
        }
        dup2(fd_in, 0);
    }

    /* Redirect output */
    if (pipeline->redirectToPath != NULL) {
        fd_out = open(pipeline->redirectToPath, O_WRONLY|O_CREAT|O_TRUNC, PERMS);
        if (fd_out == -1) {
            perror(pipeline->redirectToPath);
            if (fd_in == -1) close(fd_in);
            return -1;
        }
        dup2(fd_out, 1);
    }

    /* If the command is 'exit', clean up before executing it */
    if (strcmp(pipeline->commands[0]->argv[0], "exit") == 0) {
        /* Restore stdin and stdout */
        dup2(stdin_copy,  0);
        dup2(stdout_copy, 1);
        /* Free pipeline structure */
        pipelineFree(pipeline);
        cmd_exit(NULL);
        return -1; /* Execution won't reach here */
    }

    /* Try to execute builtin command, if it exists */
    if (!pipeline->bg && n_commands == 1 &&
            _executeBuiltinCommand(pipeline->commands[0]->argv) == 0) {
        /* Restore stdin and stdout */
        dup2(stdin_copy,  0);
        dup2(stdout_copy, 1);
        return 0;
    }

    /* Initialize pipes */
    for (i = 0; i < tot_pipes; i += 2)
        pipe(pipes+i);

    for (i = 0; i < n_commands; i++) {
        pid_t p = fork();
        if (p == 0) {
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
            closepipes(pipes, tot_pipes);
            status = executeCommand(pipeline->commands[i]);
            pipelineFree(pipeline);
            exit(status);
        }
        else
            fg_pid = launched[i] = p;

    }

    /* Only the parent gets here and waits for children to finish */
    closepipes(pipes, tot_pipes);

    /* Restore stdin and stdout */
    dup2(stdin_copy,  0);
    dup2(stdout_copy, 1);

    if (!pipeline->bg)
        for (i = 0; i < n_commands; i++)
            while (waitpid(launched[i], NULL, 0) == -1 && errno != ECHILD);
    else
        for (i = 0; i < n_commands; i++) {
            pid_counter = add_pid(launched[i]);
            printf("[%d] %d\n", pid_counter+1, launched[i]);
        }

    /* Since either the execution of the external command ended or it was
     * on the background, we have nothing running on the foreground. */
    fg_pid = 0;

    return 0;
}

// vim: et ts=4 sw=4 sts=4
