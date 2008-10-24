/* See LICENSE file for copyright and license details. */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "builtins.h"
#include "helper.h"

#define DELIMITERS " \t\n"
#define HIST_SIZE 100

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

void hellspawn(char **cmd)
{
    /* First, check if it is a builtin command */
    int i;
    for (i = 0; i < ARRAY_SIZE(commands); i++) {
        const struct cmd_struct *p = commands+i;
        if (strcmp(p->cmd, *cmd))
            continue;
        cmd++; /* Bypass command name */
        (p->fn)(cmd);
        exit(EXIT_SUCCESS);
    }

    /* If not, try to execute as an external command */
    int status = execvp(*cmd, cmd);
    if (status == -1) {
        if (errno == ENOENT)
            fprintf(stderr, "%s: command not found\n", *cmd);
        else
            perror("");
    }
    exit(status); /* If the execvp failed for some reason, exit here */
}

/* Execute an external command */
int external_exec(char **myArgv, int bg)
{
    int pid_counter;

    pid_t pid = fork();

    if (pid < 0 ) {
        perror("fork failed");
        return -1;
    }

    if (pid == 0) /* Child process */
        hellspawn(myArgv);

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

/* Closes all pipes */
void closepipes(int *pipes, int count)
{
    int i;
    for (i = 0; i < count; i++)
        close(pipes[i]);
}

/* Executes several external commands, with pipelines */
int join(char **argv, int n_commands, int bg)
{
    int i,status;
    int tot_pipes = 2*(n_commands-1); // Total pipe ends
    int pipes[tot_pipes];
    char *myArgv[n_commands][100];

    /* Tokenize given commands */
    for (i = 0; i < n_commands; i++)
        tokenize(myArgv[i], argv[i], DELIMITERS);

    /* Initialize pipes */
    for (i = 0; i < tot_pipes; i += 2)
        pipe(pipes+i);

    for (i = 0; i < n_commands; i++) {
        if (fork() == 0) {
            if (i == 0)                 /* First command */
                dup2(pipes[1], 1);
            else if (i == n_commands-1) /* Last comand */
                if (i == 1)
                    dup2(pipes[0], 0);
                else if (i%2 == 0)
                    dup2(pipes[i], 0);
                else
                    dup2(pipes[i+1], 0);
            else {                      /* Everything in between */
                if (i%2 == 1) {
                    dup2(pipes[i-1], 0);
                    dup2(pipes[i+2], 1);
                }
                else {
                    dup2(pipes[i], 0);
                    dup2(pipes[i+3], 1);
                }
            }
            closepipes(pipes, tot_pipes);
            hellspawn(myArgv[i]);
        }
    }

    /* Only the parent gets here and waits for children to finish */
    closepipes(pipes, tot_pipes);

    for (i = 0; i < n_commands; i++)
        wait(&status);

    return 0;
}

// vim: et ts=4 sw=4 sts=4
