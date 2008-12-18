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

#include "alias.h"
#include "common.h"
#include "helper.h"
#include "jobs.h"
#include "parser.h"

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
static void evil_dead(void);
static void signalHandler(int sig);
static int setupSignalHandler(void);
int main(void);

/* Executed at exit */
void cleanup()
{
    free(pids);
    destroyAliases();
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
void signalHandler(int sig)
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
int setupSignalHandler()
{
    static struct sigaction act;
    act.sa_flags = 0;
    act.sa_handler = signalHandler;
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
    setupSignalHandler();               /* Set signal handlers */
    initializeAliases();

    while ( 1 ) {
        line[0] = '\0';                 /* Clear the user input buffer */
        print_prompt();                 /* Print the prompt */
        fgets(line, BUF_SIZE, stdin);   /* Read the user input to a buffer */
        interpret_line(line, myArgv);   /* Interpret the command and make three wishes come true */
    }

    return 0;
}

// vim: et ts=4 sw=4 sts=4
