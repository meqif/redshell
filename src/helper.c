/*
 * redshell - simple and light UNIX shell
 * helper.c: helper functions
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

#include <wordexp.h>

#include "alias.h"
#include "common.h"

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
    char *aux = command;
    char *final = calloc(BUFSIZE+1, sizeof(char));
    char *ptr = strstr(command, " ");
    char *cmd;

    if (ptr == NULL)
        ptr = aux+strlen(aux);
    cmd = calloc(ptr-aux+1, sizeof(char));
    strncpy(cmd, aux, ptr-aux);

    while ((aux = getAlias(cmd)) != NULL) {
        char *tmp = calloc(BUFSIZE+1, sizeof(char));
        char *space = NULL;
        strncpy(tmp, aux, BUFSIZE);
        space = strstr(final, " ");
        if (space != NULL)
            strcat(tmp, space);
        free(final);
        final = tmp;
        tmp = NULL;

        ptr = strstr(aux, " ");
        free(cmd);
        if (ptr == NULL)
            ptr = aux+strlen(aux);
        cmd = calloc(ptr-aux+1, sizeof(char));
        strncpy(cmd, aux, ptr-aux);
    }
    free(cmd);

    if (strlen(final) == 0)
        strncpy(final, command, BUFSIZE);
    else {
        ptr = strstr(command, " ");
        if (ptr != NULL)
            strcat(final, ptr);
    }
    releaseAliases();
    return final;
}

// vim: et ts=4 sw=4 sts=4
