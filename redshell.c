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


/* Prototypes */
static void cleanup(void);
static int print_prompt(void);
static int interpret_line(char *buffer, char **myArgv);
static void evil_dead(void);
static void sig_handler(int sig);
static int set_signals(void);
int main(void);

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

    /* Should this job run in background? */
    int bg = 0;
    char *aux;
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

    command_t *cmds[n_commands];
    /* Tokenize given commands */
    int i;
    for (i = 0; i < n_commands; i++) {
        if ((aux = strstr(commands[i], "<")) != NULL) *aux = '\0';
        if ((aux = strstr(commands[i], ">")) != NULL) *aux = '\0';
        command_t *command = commandNew();
        expandize(command, commands[i]);
        command->path = command->argv[0];
        cmds[i] = command;
    }

    pipeline_t *pipeline = pipelineNew();

    pipeline->pipes            = n_commands;
    pipeline->commands         = cmds;
    pipeline->redirectToPath   = outfile;
    pipeline->redirectFromPath = infile;
    pipeline->bg               = bg;

    /* Execute the user command(s) */
    pipe_exec(pipeline);

    pipelineFree(pipeline);

    return 0;
}

/* Exorcise all zombies */
void evil_dead() {
    int i, state;
    for (i = 0; i < HIST_SIZE; i++) {
        if (pids[i] != 0) {
            state = waitpid(pids[i], NULL, WNOHANG);
            if (state > 0 || errno == ECHILD) {
                printf("\n[%d] %d done\n", i+1, pids[i]);
                pids[i] = 0;
            }
            else if (state == -1) {
                perror("");
            }
        }
    }
}

/* Deal with signals */
void sig_handler(int sig)
{
    switch(sig) {
        case SIGALRM:
            kill(-fg_pid, SIGKILL);
            fprintf(stderr, "[?] %d timeout\n", fg_pid);
            waitpid(fg_pid, NULL, 0);
            fg_pid = 0;
            break;
        case SIGINT:
            if (fg_pid != 0) {
                kill(fg_pid, sig);
                waitpid(fg_pid, NULL, 0);
                fg_pid = 0;
            }
            printf("\n");
            break;
        case SIGCHLD:
            evil_dead();
            break;
        default:
            fprintf(stderr, "Caught unexpected signal %d\n", sig);
            break;
    }
}

/* Set signal handlers */
int set_signals()
{
    static struct sigaction act;
    act.sa_handler = sig_handler;
    sigfillset(&(act.sa_mask));
    if (sigaction(SIGINT, &act, NULL) == -1)
        perror_exit("Setting SIGINT handler failed");
    if (sigaction(SIGCHLD, &act, NULL) == -1)
        perror_exit("Setting SIGCHLD handler failed");
    if (sigaction(SIGALRM, &act, NULL) == -1)
        perror_exit("Setting SIGALRM handler failed");
    return 0;
}

int main()
{
    char *myArgv[MAX_ARGS];
    char line[BUF_SIZE];                /* Buffer for user input */

    pids = calloc(HIST_SIZE, sizeof(pid_t));

    atexit(cleanup);                    /* Define some cleaning up operations */
    set_signals();                      /* Set signal handlers */

    while ( 1 ) {
        line[0] = '\0';                 /* Clear the user input buffer */
        print_prompt();                 /* Print the prompt */
        fgets(line, BUF_SIZE, stdin);   /* Read the user input to a buffer */
        interpret_line(line, myArgv);   /* Interpret the command and make three wishes come true */
    }

    return 0;
}

// vim: et ts=4 sw=4 sts=4
