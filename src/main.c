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

#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <setjmp.h>

#include "alias.h"
#include "common.h"
#include "helper.h"
#include "jobs.h"
#include "parser.h"
#include "prompt.h"

/* Readline */
#ifdef GNU_READLINE
# include <readline/readline.h>
# include <readline/history.h>
#else
# include <editline/readline.h>
#endif /* GNU_READLINE */

/*! \brief Assuming max 100 program name + args */
#define MAX_ARGS 100

/*! \brief Array of background processes' pids */
pid_t *pids;

/*! \brief Foreground process' pid */
pid_t fg_pid = 0;

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

/*! \brief Main loop */
static void run(void);

int main(void);

/********
 * Code *
 ********/

void cleanup()
{
    free(pids);
    destroyAliases();
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

void initialize_readline()
{
    /* Allow conditional parsing of the ~/.inputrc file. */
    rl_readline_name = "redshell";

    /* Our completer function */
    rl_attempted_completion_function = NULL;
}

void run()
{
    char *myArgv[MAX_ARGS];
    char *line;                         /* Buffer for user input */
    char *expansion;
    int result;

    while ( 1 ) {
        sigsetjmp(buf,1);
        line = readline(getPrompt());

        result = history_expand(line, &expansion);

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

}

int main()
{
    pids = calloc(HIST_SIZE, sizeof(pid_t));

    atexit(cleanup);                    /* Define some cleaning up operations */
    setupSignalHandler();               /* Set signal handlers */
    setPrompt(NULL);
    initializeAliases();
    run();

    return 0;
}

// vim: et ts=4 sw=4 sts=4
