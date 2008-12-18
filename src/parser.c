/* See LICENSE file for copyright and license details. */

#include <string.h>

#include "alias.h"
#include "command.h"
#include "common.h"
#include "helper.h"
#include "jobs.h"
#include "pipeline.h"

/* Interpret command array */
int interpret_line(char *buffer, char **myArgv)
{
    /* Split user input into tokens */
    char bufcopy[strlen(buffer)+1];
    strcpy(bufcopy, buffer);
    tokenize(myArgv, bufcopy, DELIMITERS);

    /* Get command name */
    char *cmd = *myArgv;

    /* Do nothing if we get a blank line */
    if (cmd == NULL) return -1;

    pipeline_t *pipeline = pipelineNew();

    /* Number of commands = number of pipes + 1 */
    pipeline->pipes = strstrcnt(buffer, '|')+1;
    char *commands[pipeline->pipes];

    /* Should this job run in background? */
    char *aux;
    if ((aux = strstr(buffer, "&")) != NULL) {
        pipeline->bg = 1;
        *aux = '\0';
    }

    /* Check if the user wants to redirect the input or output */
    findRedirections(pipeline, myArgv);

    /* Split user input by pipe */
    tokenize(commands, buffer, "|\n");

    command_t *cmds[pipeline->pipes];
    /* Tokenize given commands */
    int i;
    for (i = 0; i < pipeline->pipes; i++) {
        if ((aux = strstr(commands[i], "<")) != NULL) *aux = '\0';
        if ((aux = strstr(commands[i], ">")) != NULL) *aux = '\0';
        char *a = expandAlias(commands[i]);
        releaseAliases();
        command_t *command = commandNew();
        expandGlob(command, a);
        command->path = command->argv[0];
        cmds[i] = command;
        free(a);
    }
    pipeline->commands = cmds;

    /* Execute the user command(s) */
    spawnCommand(pipeline);

    pipelineFree(pipeline);

    return 0;
}

// vim: et ts=4 sw=4 sts=4
