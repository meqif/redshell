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
int external_exec(char **myArgv, int bg, char *infile, char *outfile)
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

    if (pid == 0) { /* Child process */
        if (infile != NULL) {
            int fd_in = open(infile, O_RDONLY);
            dup2(fd_in, 0);
        }
        if (outfile != NULL) {
            int fd_out = open(outfile, O_WRONLY|O_CREAT|O_TRUNC, 0644);
            dup2(fd_out, 1);
        }
        executioner(myArgv);
    }

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
int join(char **argv, int n_commands, int bg, char *infile, char *outfile)
{
    int i;
    int fd_in = -1, fd_out = -1;
    int tot_pipes = 2*(n_commands-1); // Total pipe ends
    int pipes[tot_pipes];
    char *myArgv[n_commands][100];
    char *aux;
    pid_t launched[n_commands];

    if (infile != NULL) {
        fd_in = open(infile, O_RDONLY);
        dup2(fd_in, 0);
    }
    if (outfile != NULL) {
        fd_out = open(outfile, O_WRONLY|O_CREAT|O_TRUNC, 0644);
        dup2(fd_out, 1);
    }

    /* Tokenize given commands */
    for (i = 0; i < n_commands; i++) {
        if ((aux = strstr(argv[i], "<")) != NULL) *aux = '\0';
        if ((aux = strstr(argv[i], ">")) != NULL) *aux = '\0';
        tokenize(myArgv[i], argv[i], DELIMITERS);
    }

    /* Try to execute builtin command, if it exists */
    if (!bg && n_commands == 1 && builtin_exec(myArgv[0]) == 0)
        return 0;

    /* Initialize pipes */
    for (i = 0; i < tot_pipes; i += 2)
        pipe(pipes+i);

    for (i = 0; i < n_commands; i++) {
        pid_t p = fork();
        if (p == 0) {
            if (i == 0) {               /* First command */
                dup2(pipes[1], 1);
                if (fd_in != -1) dup2(fd_in, 0);
            }
            else if (i == n_commands-1) { /* Last comand */
                if (i == 1)
                    dup2(pipes[0], 0);
                else if (i%2 == 0)
                    dup2(pipes[i], 0);
                else
                    dup2(pipes[i+1], 0);
                if (fd_out != -1) dup2(fd_out, 1);
            }
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
            executioner(myArgv[i]);
        }
        else
            launched[i] = p;
    }

    /* Only the parent gets here and waits for children to finish */
    closepipes(pipes, tot_pipes);

    if (!bg)
        for (i = 0; i < n_commands; i++) {
            waitpid(launched[i], NULL, 0);
            fg_pid = launched[i];
        }
    else
        for (i = 0; i < n_commands; i++) {
            int pid_counter = add_pid(launched[i]);
            printf("[%d] %d\n", pid_counter+1, launched[i]);
        }

    /* Since either the execution of the external command ended or it was
     * on the background, we have nothing running on the foreground. */
    fg_pid = 0;

    return 0;
}

int run_command(char *buffer)
{
    int bg = 0;
    char *aux;

    /* Should this job run in background? */
    if ((aux = strstr(buffer, "&")) != NULL) {
        bg = 1;
        *aux = '\0';
    }

    /* Number of commands = number of pipes + 1 */
    int n_commands = strstrcnt(buffer, '|')+1;
    char *commands[n_commands];

    /* Split user input into tokens */
    char *myArgv[100];
    char bufcopy[strlen(buffer)+1];
    strcpy(bufcopy, buffer);
    tokenize(myArgv, bufcopy, DELIMITERS);

    int counter = 0;
    char *infile = NULL;
    char *outfile = NULL;
    while(*(myArgv+counter) != NULL) {
        char **aux = myArgv+counter;
        if ((strcmp(*aux, ">") == 0) && *(aux+1) != NULL) {
            *aux = NULL;
            outfile = strdup(*(++aux));
            *aux = NULL;
            counter++;
        }
        else if ((strcmp(*aux, "<") == 0) && *(aux+1) != NULL) {
            *aux = NULL;
            infile = strdup(*(++aux));
            *aux = NULL;
            counter++;
        }
        counter++;
    }


    /* Save original stdin and stdout */
    int cp_in  = -1;
    int cp_out = -1;
    cp_in  = dup(0);
    cp_out = dup(1);

    tokenize(commands, buffer, "|\n");
    join(commands, n_commands, bg, infile, outfile);

    /* Restore stdin and stdout */
    dup2(cp_in,  0);
    dup2(cp_out, 1);

    return 0;
}

// vim: et ts=4 sw=4 sts=4
