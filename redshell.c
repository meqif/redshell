/* See LICENSE file for copyright and license details. */

/*
 * I'm the ghost in your shell, I'm the -9 in your kill,
 * I'm the unexplainable memory corruption that won't let you sleep at night.
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

#include "common.h"
#include "helper.h"
#include "jobs.h"

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
    char *aux;
    int bg = 0;

    /* Should this job run in background? */
    if ((aux = strstr(buffer, "&")) != NULL) {
        bg = 1;
        *aux = '\0';
    }

    /* TODO: Handle redirections */
    /* tokenize? */

    char bufcopy[strlen(buffer)+1];
    strcpy(bufcopy, buffer);
    tokenize(myArgv, bufcopy, DELIMITERS);
    const char *cmd = *myArgv;

    /* Do nothing if we get a blank line */
    if (cmd == NULL) return -1;

    run_command(buffer, bg);

    return 0;
}

/* Exorcise all zombies */
void evil_dead() {
    int i, state;
    for (i = 0; i < HIST_SIZE; i++) {
        if (pids[i] != 0) {
            state = waitpid(pids[i], NULL, WNOHANG);
            if (state > 0) {
                printf("[%d]  %d done\n", i+1, pids[i]);
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
    char *myArgv[100];                 /* Assumed max 100 program name + args */
    char line[BUF_SIZE];               /* Buffer for user input */

    pids = calloc(HIST_SIZE, sizeof(pid_t));

    atexit(cleanup);                   /* Define some cleaning up operations */

    /* Signal stuff */
    static struct sigaction act;
    act.sa_handler = handle_sigint;
    sigfillset(&(act.sa_mask));
    sigaction(SIGINT, &act, NULL);

    while ( 1 ) {
        evil_dead();                    /* Deal with possible zombies */
        print_prompt();                 /* Print the prompt */
        fgets(line, BUF_SIZE, stdin);   /* Read the user input to a buffer */
        interpret_line(line, myArgv);   /* Interpret the command and make three wishes come true */
    }

    return 0;
}

// vim: et ts=4 sw=4 sts=4
