/* See LICENSE file for copyright and license details. */

/*
 * I'm the ghost in your shell, I'm the -9 in your kill,
 * I'm the unexplainable memory corruption that won't let you sleep at night.
 */

/*
 * TODO:
 *  - History
 *  - Redirection and pipes
 *  - Tab completion
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

#include "builtins.h"

#define DELIMITERS " \t\n"
#define BUF_SIZE 1000
#define HIST_SIZE 100

/* Shell colours */
#define RED      "\033[1;31m"
#define CLEAR    "\033[0m"

pid_t *pids;
pid_t fg_pid = 0;

/* Executed at exit */
void cleanup()
{
    free(pids);
}

/* Add pid to the first empty index in pids array */
int add_pid(pid_t new_pid)
{
    int i;
    for (i = 0; i < HIST_SIZE; i++) {
        if (pids[i] == 0) {
            pids[i] = new_pid;
            return i;
        }
    }
    return -1;
}

/* Execute an external command */
int external_exec(char **myArgv)
{
    int bg = 0;
    int counter = 0;
    int pid_counter;

    while (*(myArgv+counter) != NULL) {
        if (!strcmp(*(myArgv+counter), "&")) {
            bg = 1;
            *(myArgv+counter) = NULL;
            break;
        }
        counter++;
    }

    pid_t pid = fork();

    if (pid < 0 ) {
        perror("fork failed");
        return -1;
    }

    if (pid == 0) { /* Child process */
        int status = execvp(*myArgv, myArgv);
        if (status == -1) {
            if (errno == ENOENT)
                fprintf(stderr, "%s: command not found\n", *myArgv);
            else
                perror("");
        }
        exit(status);
    }

    if (bg) {
        pid_counter = add_pid(pid);
        printf("[%d] %d\n", pid_counter+1, pid);
    }
    else {
        fg_pid = pid;
        wait(NULL);
    }

    /* Since either the execution of the external command ended or it was
     * on the background, we have nothing running on the foreground. */
    fg_pid = 0;

    return 0;
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

/* Split the user input into tokens */
int read_line(char *line, char **myArgv)
{
    char *result = NULL;
    result = strtok(line, DELIMITERS);

    while (result != NULL) {
        *(myArgv++) = result;
        result = strtok(NULL, DELIMITERS);
    }

    /* End of array */
    *myArgv = NULL;

    free(result);

    return 0;
}

/* Interpret command array */
int interpret_line(char **myArgv)
{
    if (*myArgv == NULL)
        return -1;

    else if ( strcmp(*myArgv, "exit") == 0 ) {
        printf("Bye!\n");
        exit(0);
    }

    else if ( strcmp(*myArgv, "eco") == 0 )
        eco(myArgv);

    else if ( strcmp(*myArgv, "pwd") == 0 )
        pwd();

    else if ( strcmp(*myArgv, "cd") == 0 )
        cd(*(++myArgv));

    else if ( strcmp(*myArgv, "listar") == 0 )
        listar(*(++myArgv));

    else
        external_exec(myArgv);

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
        }
    }
}

/* Pass SIGINTs to the foreground process */
void handle_sigint(int sig)
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
        fgets(line, BUF_SIZE, stdin);   /* Read the user input to a buffer (line) */
        read_line(line, myArgv);        /* Split that input into delicious tokens */
        interpret_line(myArgv);         /* Interpret the tokens and make three wishes come true */
    }

    return 0;
}

// vim: et ts=4 sw=4 sts=4
