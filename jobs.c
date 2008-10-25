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

#include "builtins.h"
#include "common.h"
#include "helper.h"

extern pid_t fg_pid;

/* Run given builtin command, if possible */
int builtin_exec(char **cmd)
{
    int i;
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
void executioner(char **cmd)
{
    /* First, check if it is a builtin command */
    if (builtin_exec(cmd) == 0) exit(EXIT_SUCCESS);

    /* If not, try to execute as an external command */
    int status = execvp(*cmd, cmd);
    if (status == -1) {
        if (errno == ENOENT)
            fprintf(stderr, "%s: command not found\n", *cmd);
        else
            perror(*cmd);
    }
    exit(status); /* If the execvp failed for some reason, exit here */
}

/* Execute an external command */
int external_exec(char **myArgv, int bg)
{
    int pid_counter;

    /* Try to execute builtin command, if it exists */
    if (!bg && builtin_exec(myArgv) == 0)
        return 0;

    pid_t pid = fork();

    if (pid < 0 ) {
        perror("fork failed");
        return -1;
    }

    if (pid == 0) /* Child process */
        executioner(myArgv);

    if (bg) {
        pid_counter = add_pid(pid);
        printf("[%d] %d\n", pid_counter+1, pid);
    }
    else {
        fg_pid = pid;
        waitpid(pid, NULL, 0);
    }

    /* Since either the execution of the external command ended or it was
     * on the background, we have nothing running on the foreground. */
    fg_pid = 0;

    return 0;
}

// vim: et ts=4 sw=4 sts=4
