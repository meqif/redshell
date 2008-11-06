/*
 * redshell - simple and light UNIX shell
 * helper.c: helper functions
 *
 * Copyright (C) 2008 Ricardo Martins <ricardo@scarybox.net>
 * See LICENSE or http://www.opensource.org/licenses/mit-license.php
 * for copyright and license details.
 */

#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

#include "common.h"

extern pid_t *pids;

/* Counts the ocurrences of a given char in a given string */
int strstrcnt(char *str, char ch)
{
    int counter = 0;
    while (*str != '\0') {
        if (*str == ch)
            counter++;
        str++;
    }
    return counter;
}

/* Convert uid to username */
char *getusername(uid_t uid)
{
    struct passwd *user = getpwuid(uid);
    char *username = user->pw_name;
    return username;
}

/* Convert gid to group name */
char *getgroupname(gid_t gid)
{
    struct group *grp = getgrgid(gid);
    char *groupname = grp->gr_name;
    return groupname;
}

/* Split a string into tokens */
void tokenize(char **dst, char *buffer, const char *delimiters)
{
    char *result = strtok(buffer, delimiters);

    while (result != NULL) {
        *(dst++) = result;
        result = strtok(NULL, delimiters);
    }
    *dst = NULL; /* NULL terminate the array */

    free(result);
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

/* Replace '~' with the contents of $HOME */
char *expand_tilde(char *dest, char *src)
{
    char *tilde = NULL;
    tilde = strstr(src, "~");

    if (tilde != NULL) {
        *tilde = '\0';
        strcat(dest, src);
        strcat(dest, getenv("HOME"));
        src = tilde+1;
        strcat(dest, src);
    }
    else
        strcat(dest, src);
    return dest;
}

/* Expand environment variables */
int expand_env(char **argv)
{
    char **tmp = argv;
    while(*tmp != NULL) {
        char *dollar = strstr(*tmp, "$");
        if (dollar != NULL) {
            char *new = calloc(1000, 1);
            int len = dollar-(*tmp);
            strncpy(new, *tmp, len);
            char *p = strstr(dollar, "/");
            if (p != NULL) {
                int len2 = p-dollar;
                char *word = calloc(len2,1);
                strncpy(word, dollar+1, len2-1);
                char *envvar = getenv(word);
                free(word);
                if (envvar != NULL)
                    strcat(new, envvar);
                strcat(new, dollar+len2);
            }
            else {
                char *envvar = getenv(dollar+1);
                if (envvar != NULL)
                    strcat(new, envvar);
            }
            *tmp = new;
            free(new);
        }
        tmp++;
    }
    return 0;
}

int expand_env2(char **argv)
{
    while (*argv != NULL) {
        if (strstr(*argv, "$")) { /* Do we really need to mangle the argument? */
            char *head, *tok, *word;
            /* head will point to where we are in the string, tok will point to
             * the beggining of the last token */
            head = tok = *argv;
            char *new = calloc(1000,1);
            int stop = 0;
            do {
                if (*head == 0) stop = 1;
                if (*head == '$' || *head == '/' || *head == 0) {
                    word = calloc(head-tok+1,1);  /* Get the word since the last */
                    strncat(word, tok, head-tok); /* symbol to the current one   */
                    if (getenv(word) != NULL)
                        strcat(new, getenv(word));
                    else
                        strcat(new, word);
                    free(word);
                    if (*head == '/')
                        strcat(new, "/");
                    *head = 0;             /* Separate tokens */
                    tok = head+1;          /* Ignore current character ('\0') */
                }
                head++;
            } while (!stop);
            if (*new != 0)
                *argv = new;
            free(new);
        }
        argv++;
    }
    return 0;
}

void perror_exit(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
// vim: et ts=4 sw=4 sts=4
