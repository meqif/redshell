/*
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

#include <assert.h>

#include "command.h"
#include "common.h"

command_t *commandNew(void)
{
    command_t *command = NULL;
    command = malloc(sizeof(*command));
    if(command == NULL) {
        return NULL;
    }
    command->path = NULL;
    command->argc = 0;
    command->argv = NULL;
    command->redirectToPath = NULL;
    command->redirectFromPath = NULL;
    command->connectionMask = commandConnectionNone;

    return command;
}

void commandFree(command_t *command)
{
    int i = 0;
    assert(command != NULL);
    if (command->argv != NULL) {
        while(command->argv[i] != NULL) {
            free(command->argv[i]);
            command->argv[i++] = NULL;
        }
        free(command->argv);
        command->argv = NULL;
        command->path = NULL;
    }
    /* path is an alias for argv[0], which has already been freed */
    if(command->redirectToPath != NULL) {
        free(command->redirectToPath);
        command->redirectToPath = NULL;
    }
    if(command->redirectFromPath != NULL) {
        free(command->redirectFromPath);
        command->redirectFromPath = NULL;
    }
    free(command);
}

// vim: et ts=4 sw=4 sts=4
