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

#include "alias.h"
#include "common.h"

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

/* Expand single-string input */
/* eg. $HOME/src -> /home/user/src */
char *expand(char *input)
{
    wordexp_t p;
    char **w, *result;

    result = NULL;
    if (wordexp(input, &p, 0) == 0) {
        w = p.we_wordv;
        p.we_offs = 0; /* Fix for OSX 10.5 bug in wordfree */
        if (p.we_wordc == 1)
            result = strdup(w[0]);
        wordfree(&p);
    }
    return result;
}

/* Expand user input and store each word in an array */
int expandGlob(command_t *command, char *cmd)
{
    wordexp_t p;
    char **w;
    unsigned int i;

    if (wordexp(cmd, &p, 0) == 0) {
        w = p.we_wordv;
        p.we_offs = 0; /* Fix for OSX 10.5 bug in wordfree */
        command->argv = calloc(p.we_wordc+1, sizeof(char *));
        for (i = 0; i < p.we_wordc; i++)
            command->argv[i] = strdup(w[i]);
        command->argv[i] = NULL;
        wordfree(&p);
        return 0;
    }
    else
        return -1;
}

/* Expand aliased command */
char *expandAlias(char *command)
{
    char *aux;
    char *final = calloc(BUFSIZE, 1);
    char *ptr = strstr(command, " ");
    char *cmd;

    if (ptr != NULL) {
        cmd = calloc(ptr-command+1, sizeof(char));
        strncpy(cmd, command, ptr-command);
    }
    else
        cmd = strdup(command);

    while ((aux = getAlias(cmd)) != NULL) {
        char *tmp = calloc(BUFSIZE, 1);
        char *space = NULL;
        strcpy(tmp, aux);
        if (final != NULL && strlen(final) > 0)
            space = strstr(final, " ");
        if (space != NULL)
            strcat(tmp, space);
        free(final);
        final = tmp;
        tmp = NULL;

        ptr = strstr(aux, " ");
        free(cmd);
        if (ptr != NULL) {
            cmd = calloc(ptr-aux+1, sizeof(char));
            strncpy(cmd, aux, ptr-aux);
        } else
            cmd = strdup(aux);
    }

    free(cmd);

    if (strlen(final) == 0)
        strcpy(final, command);
    else {
        ptr = strstr(command, " ");
        if (ptr != NULL) {
            strcat(final, " ");
            strcat(final, ptr+1);
        }
    }
    releaseAliases();
    return final;
}

// vim: et ts=4 sw=4 sts=4
