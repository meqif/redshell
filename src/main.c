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

#include "alias.h"
#include "common.h"
#include "jobs.h"
#include "parser.h"
#include "prompt.h"
#include "queue.h"

/* Readline */
#ifdef GNU_READLINE
# include <readline/readline.h>
# include <readline/history.h>
#else
# include <editline/readline.h>
#endif /* GNU_READLINE */

/*! \brief Foreground process' pid */
pid_t fg_pid = 0;

/**************
 * Prototypes *
 **************/

/*! \brief Memory cleaning up */
static void cleanup(void);

/*! \brief Handle signals */
static void signalHandler(int sig);

/*! \brief Set signal handlers */
static int setupSignalHandler(void);

/*! \brief Main loop */
static void run(void);

/*! \brief Setup readline */
static void initializeReadline(void);

int main(void);

/********
 * Code *
 ********/

void cleanup()
{
    destroyAliases();
}

void signalHandler(int sig)
{
    int state;
    switch(sig) {
        case SIGINT:
            if (fg_pid != 0) {
                kill(fg_pid, sig);
                waitpid(fg_pid, NULL, 0);
                fg_pid = 0;
            }
            printf("\n");
            break;
        case SIGCHLD:
            state = waitpid(0, NULL, WNOHANG);
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
    return ( sigaction(SIGINT,  &act, NULL) == -1 ||
             sigaction(SIGCHLD, &act, NULL) == -1 );
}

void initializeReadline()
{
    /* Allow conditional parsing of the ~/.inputrc file. */
    rl_readline_name = "redshell";

    /* Our completer function */
    rl_attempted_completion_function = NULL;
}

void run()
{
    char *line;                         /* Buffer for user input */
    char *expansion;
    int result;
    queue_t *commandQueue;

    while ( 1 ) {
        line = readline(getPrompt());
        result = history_expand(line, &expansion);

        if (result < 0 || result == 2)
            fprintf(stderr, "%s\n", expansion);
        else {
            commandQueue = parseInput(expansion);
            if (commandQueue->count > 0)
                add_history(expansion);
            executeCommandsInQueue(commandQueue);
            queueFree(commandQueue);
        }

        free(expansion);
        free(line);
    }

}

int main()
{
    atexit(cleanup);                    /* Define some cleaning up operations */
    if (setupSignalHandler()) {         /* Set signal handlers */
        fprintf(stderr, "Failed setting up signals, exiting...\n");
        return EXIT_FAILURE;
    }
    setPrompt(NULL);
    initializeAliases();
    initializeReadline();
    run();

    return 0;
}

// vim: et ts=4 sw=4 sts=4
