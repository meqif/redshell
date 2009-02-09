/*
 * redshell - simple and light UNIX shell
 *
 * I'm the ghost in your shell, I'm the -9 in your kill,
 * I'm the unexplainable memory corruption that won't let you sleep at night.
 *
 * Copyright (c) 2008-2009 Ricardo Martins <ricardo@scarybox.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
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
            waitpid(0, NULL, WNOHANG);
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
