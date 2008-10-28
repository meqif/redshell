/*
 * redshell - simple and light UNIX shell
 *
 * I'm the ghost in your shell, I'm the -9 in your kill,
 * I'm the unexplainable memory corruption that won't let you sleep at night.
 *
 * Copyright (C) 2008 Ricardo Martins <ricardo@scarybox.net>
 * See LICENSE or http://www.opensource.org/licenses/mit-license.php
 * for copyright and license details.
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

#include "common.h"
#include "helper.h"
#include "jobs.h"

#define MAX_ARGS 100        /* Assuming max 100 program name + args */
/* Shell colours */
#define RED      "\033[1;31m"
#define CLEAR    "\033[0m"

/* Array of background processes' pids */
pid_t *pids;
/* Foreground process' pid */
pid_t fg_pid = 0;

/* Executed at exit */
void cleanup()
{
    free(pids);
}

/* Print the shell prompt */
int print_prompt()
{
    char *username = getusername(getuid());
    char hostname[100];

    gethostname(hostname, 100);
    printf("%s%s@%s > %s", RED, username, hostname, CLEAR);

    return 0;
}

/* Interpret command array */
int interpret_line(char *buffer, char **myArgv)
{
    /* Split user input into tokens */
    char bufcopy[strlen(buffer)+1];
    strcpy(bufcopy, buffer);
    tokenize(myArgv, bufcopy, DELIMITERS);

    /* Get command name */
    const char *cmd = *myArgv;

    /* Do nothing if we get a blank line */
    if (cmd == NULL) return -1;

    /* Number of commands = number of pipes + 1 */
    int n_commands = strstrcnt(buffer, '|')+1;
    char *commands[n_commands];

    int bg = 0;
    char *aux;

    /* Should this job run in background? */
    if ((aux = strstr(buffer, "&")) != NULL) {
        bg = 1;
        *aux = '\0';
    }

    /* Check if the user wants to redirect the input or output */
    char *infile = NULL;
    char *outfile = NULL;
    char **tmp = myArgv;
    while(*tmp != NULL) {
        if ((strcmp(*tmp, ">") == 0) && *(tmp+1) != NULL)
            outfile = *(++tmp);
        else if ((strcmp(*tmp, "<") == 0) && *(tmp+1) != NULL)
            infile = *(++tmp);
        tmp++;
    }

    /* Split user input by pipe */
    tokenize(commands, buffer, "|\n");

    /* Execute the user command(s) */
    pipe_exec(commands, n_commands, bg, infile, outfile);

    return 0;
}

/* Exorcise all zombies */
void evil_dead() {
    int i, state;
    for (i = 0; i < HIST_SIZE; i++) {
        if (pids[i] != 0) {
            state = waitpid(pids[i], NULL, WNOHANG);
            if (state > 0) {
                printf("[%d] %d done\n", i+1, pids[i]);
                pids[i] = 0;
            }
            else if (state == -1) {
                if (errno == ECHILD) /* Someone else killed the child */
                    pids[i] = 0;
                else
                    perror("");
            }
        }
    }
}

/* Pass SIGINTs to the foreground process */
void handle_sigint()
{
    if (fg_pid != 0) kill(fg_pid, SIGINT);
    printf("\n");
}

int main()
{
    char *myArgv[MAX_ARGS];
    char line[BUF_SIZE];               /* Buffer for user input */

    pids = calloc(HIST_SIZE, sizeof(pid_t));

    atexit(cleanup);                   /* Define some cleaning up operations */

    /* Signal stuff */
    static struct sigaction act;
    act.sa_handler = handle_sigint;
    sigfillset(&(act.sa_mask));
    sigaction(SIGINT, &act, NULL);

    while ( 1 ) {
        line[0] = '\0';                 /* Clear the user input buffer */
        evil_dead();                    /* Deal with possible zombies */
        print_prompt();                 /* Print the prompt */
        fgets(line, BUF_SIZE, stdin);   /* Read the user input to a buffer */
        interpret_line(line, myArgv);   /* Interpret the command and make three wishes come true */
    }

    return 0;
}

// vim: et ts=4 sw=4 sts=4
