/* See LICENSE file for copyright and license details. */

#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

#include "common.h"

extern pid_t *pids;

/* Prints an error message and exits */
void fatal(char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

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

/* Removes (NULLs) the last element of an array of pointers */
void remove_last(char *arr[])
{
    int i = 0;
    while(arr[i] != NULL) i++;
    arr[--i] = NULL;
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

// vim: et ts=4 sw=4 sts=4
