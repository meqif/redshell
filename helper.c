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
#include <wordexp.h>

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

void expandGlob(command_t *command, char *cmd)
{
    wordexp_t p;
    char **w;
    unsigned int i;

    wordexp(cmd, &p, 0);
    w = p.we_wordv;
    p.we_offs = 0; /* Fix for OSX 10.5 bug in wordfree */
    command->argv = calloc(p.we_wordc+1, sizeof(char *));
    for (i=0; i < p.we_wordc; i++)
        command->argv[i] = strdup(w[i]);
    command->argv[i] = NULL;
    wordfree(&p);
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

void perror_exit(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

// vim: et ts=4 sw=4 sts=4
