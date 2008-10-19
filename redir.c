/* See LICENSE file for copyright and license details. */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include "builtins.h"

#define DELIMITERS " \t\n"

void fatal(char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

/* Counts the ocurrences of a given char in a given string */
int strstrcnt(char *str, char ch)
{
    int counter = 0;
    while (*str != '\0') {
        if (*str == ch)
            counter++;
        str++;
    }
    return counter;
}

/* Spawns a process with specific stdout and stdin */
void spawn_child(char **command, int pipe_in, int pipe_out, int pipe[2])
{
    dup2(pipe_in, 0);
    dup2(pipe_out, 1);

    close(pipe[0]); pipe[0] = -1;
    close(pipe[1]); pipe[1] = -1;

    execvp(command[0], command);
    perror(command[0]);
    exit(EXIT_FAILURE);
}

void closepipes(int *pipes, int count)
{
    int i;
    for (i = 0; i < count; i++)
        close(pipes[i]);
}

int join(char **argv, int count)
{
    int i,status;
    char *myArgv[count][100];

    int bg = 0;

    for (i = 0; i < count; i++)
        tokenize(myArgv[i], argv[i], DELIMITERS);

    i--;
    int counter = 0;
    while (*(myArgv[i]+counter) != NULL) {
        if (strncmp(*(myArgv[i]+counter), "&", 1) == 0) {
            bg = 1;
            *(myArgv[i]+counter) = NULL;
            break;
        }
        counter++;
    }

    int total = count;
    int tot_pipes = 2*(total-1); // Total pipe ends
    int pipes[tot_pipes];
    for (i = 0; i < tot_pipes; i += 2)
        pipe(pipes+i);

    for (i = 0; i < total; i++) {
        if (fork() == 0) {
            if (i == 0)            // Primeiro comando
                dup2(pipes[1], 1);
            else if (i == total-1) // Último comando
                if (i == 1)
                    dup2(pipes[0], 0);
                else if (i%2 == 0)
                    dup2(pipes[i], 0);
                else
                    dup2(pipes[i+1], 0);
            else {                 // Tudo o resto
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
            execvp(*myArgv[i], myArgv[i]);
        }
    }

    // only the parent gets here and waits for children to finish
    closepipes(pipes, tot_pipes);

    if (!bg)
        for (i = 0; i < total; i++)
            wait(&status);

    return 0;
}

// vim: et ts=4 sw=4 sts=4
