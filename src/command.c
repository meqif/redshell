/* See LICENSE file for copyright and license details. */

#include <assert.h>

#include "command.h"
#include "common.h"

command_t *commandNew()
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
