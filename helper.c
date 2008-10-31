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
        if (**tmp == '$') {
            char expanded[strlen(*tmp)+100];
            char var[50];
            int i = 0;
            (*tmp)++;
            while (**tmp != '\0' && **tmp != '/') {
                var[i++] = **tmp;
                (*tmp)++;
            }
            var[i] = '\0';
            expanded[0] = '\0';
            if (getenv(var) != NULL)
                strcat(expanded, getenv(var));
            else
                strcat(expanded, "$");
            if (*tmp != NULL)
                strcat(expanded, *tmp);
            *tmp = expanded;
        }
        tmp++;
    }
    return 0;
}
// vim: et ts=4 sw=4 sts=4
