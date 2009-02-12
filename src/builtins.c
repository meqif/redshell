/*
 * redshell - simple and light UNIX shell
 * builtins.c: builtin commands
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

#include <errno.h>

#include "alias.h"
#include "builtins.h"
#include "common.h"
#include "helper.h"

/* Display a line of text */
int cmd_echo(char **argv)
{
    while ( *argv != NULL ) {
        printf("%s", *(argv++));
        if (*argv != NULL) printf(" ");
    }
    printf("\n");
    return 0;
}

/* Export/set environment variables */
int cmd_export(char **argv)
{
    int status;
    char *envvar = strdup(*argv);
    char *key = envvar;
    char *value = strstr(envvar, "=");
    if (value == NULL) {
        free(envvar);
        return -1;
    }
    *value = '\0';
    value++;
    status = setenv(key, value, 1);
    free(envvar);

    return status;
}

/* Display the current directory */
int cmd_pwd(char **argv)
{
    (void) argv; /* avoid warning about unused parameter */
    char buffer[BUFSIZE];
    char *dir = getcwd(buffer, BUFSIZE);

    printf("%s\n", dir);

    return 0;
}

/* Change directory */
int cmd_cd(char **argv)
{
    const char *path = *argv;
    int status;

    /* Assume the user's home directory as path if none given */
    if (path == NULL)
        path = getenv("HOME");

    status = chdir(path);

    if ( status == -1 )
        perror(path);

    return status;
}

/* Exit shell */
int cmd_exit(char **argv)
{
    (void) argv; /* avoid warning about unused parameter */
    printf("Bye!\n");
    exit(EXIT_SUCCESS);
}

int cmd_alias(char **argv)
{
    if (*argv == NULL) {
        printAliases();
        return 0;
    }
    char *alias = strdup(*argv);
    char *key;
    char *value = strstr(alias, "=");
    if (value == NULL) {
        free(alias);
        return -1;
    }
    *value = '\0';
    value++;
    key = strdup(alias);
    value = strdup(value);
    free(alias);
    addAlias(key, value);
    return 0;
}

int cmd_unalias(char **argv)
{
    if (*argv == NULL) {
        fprintf(stderr, "unalias: not enough arguments\n");
        return -1;
    }
    removeAlias(*argv);
    return 0;
}

int isBuiltin(char *cmd)
{
    unsigned int i;
    for (i = 0; i < ARRAY_SIZE(commands); i++) {
        const struct cmd_struct *p = commands+i;
        if (strcmp(p->cmd, cmd))
            continue;
        return 1;
    }
    return 0;
}

// vim: et ts=4 sw=4 sts=4
