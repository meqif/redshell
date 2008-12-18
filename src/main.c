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
#include <setjmp.h>

#include "alias.h"
#include "common.h"
#include "helper.h"
#include "jobs.h"
#include "parser.h"

/* Readline */
/* TODO: ifdef for GNU readline vs editline */
#include <editline/readline.h>

/*! \brief Assuming max 100 program name + args */
#define MAX_ARGS 100
/* Shell colours */
#define RED      "\033[1;31m"
#define CLEAR    "\033[0m"

/*! \brief Array of background processes' pids */
pid_t *pids;

/*! \brief Foreground process' pid */
pid_t fg_pid = 0;

static char *prompt;
jmp_buf buf;

/**************
 * Prototypes *
 **************/

/*! \brief Memory cleaning up */
static void cleanup(void);

/*! \brief Exorcise all zombies */
static void evil_dead(void);

/*! \brief Handle signals */
static void signalHandler(int sig);

/*! \brief Set signal handlers */
static int setupSignalHandler(void);
int main(void);

/********
 * Code *
 ********/

void cleanup()
{
    free(pids);
    destroyAliases();
}

int setPrompt(char *format)
{
    if (format == NULL) {
        char *username = getusername(getuid());
        char hostname[100];
        char *tmp = malloc(200 * sizeof(char));

        gethostname(hostname, 100);
        snprintf(tmp, 200, "%s%s@%s > %s", RED, username, hostname, CLEAR);

        free(prompt);
        prompt = malloc(strlen(tmp)+1);
        strcpy(prompt, tmp);
        free(tmp);
    } else {
        prompt = format;
    }

    return 0;
}

char *getPrompt()
{
    return prompt;
}

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
            siglongjmp(buf,1);
            break;
        case SIGCHLD:
            evil_dead();
            break;
        default:
            fprintf(stderr, "Caught unexpected signal %d\n", sig);
            break;
    }
}

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
    char *line;                         /* Buffer for user input */

    pids = calloc(HIST_SIZE, sizeof(pid_t));

    atexit(cleanup);                    /* Define some cleaning up operations */
    setupSignalHandler();               /* Set signal handlers */
    setPrompt(NULL);
    initializeAliases();

    while ( 1 ) {
        sigsetjmp(buf,1);
        line = readline(getPrompt());

        char *expansion;
        int result = history_expand(line, &expansion);

        if (expansion == NULL || strlen(expansion) == 0)
            ;
        else if (result < 0 || result == 2)
            fprintf(stderr, "%s\n", expansion);
        else {
            add_history(expansion);
            interpret_line(expansion, myArgv);
        }

        free(expansion);
        free(line);
    }

    return 0;
}

// vim: et ts=4 sw=4 sts=4
