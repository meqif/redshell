/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include <string.h>

#include "alias.h"
#include "command.h"
#include "common.h"
#include "helper.h"
#include "queue.h"

#define BUFLEN 1024

struct params
{
    char buffer[BUFLEN+1];
    int buflen;
    int cs;
};

%%{
    machine redirector;
    access fsm->;

    # Actions
    action append {
        if (fsm->buflen < BUFLEN)
            fsm->buffer[fsm->buflen++] = fc;
    }

    action term_in {
        if (fsm->buflen < BUFLEN)
            fsm->buffer[fsm->buflen++] = 0;
        paths[0] = strdup(fsm->buffer);
    }

    action term_out {
        if (fsm->buflen < BUFLEN)
            fsm->buffer[fsm->buflen++] = 0;
        paths[1] = strdup(fsm->buffer);
    }

    action clear { fsm->buflen = 0; }

    stdin  = space* ^(0|"<"|">"|space)+ >clear $append %term_in;
    stdout = space* ^(0|"<"|">"|space)+ >clear $append %term_out;

    main := any* ("<" stdin)? (">" stdout)? space* 0;

}%%

%% write data;

char *getRedirectionPaths(char *line, char *paths[])
{
    const char *p  = line;
    const char *pe = line + strlen(line)+1;
    struct params *fsm = malloc(sizeof(struct params));
    char *ptr;

    paths[0] = NULL;
    paths[1] = NULL;

    %% write init;
    %% write exec;

    /* FIXME */
    ptr = strstr(line, ">");
    if (ptr != NULL) *ptr = 0;
    ptr = strstr(line, "<");
    if (ptr != NULL) *ptr = 0;

    free(fsm);
    return NULL;
}

%%{
    machine parser;
    access fsm->;

    # Actions
    action append {
        if (fsm->buflen < BUFLEN)
            fsm->buffer[fsm->buflen++] = fc;
    }

    action term {
        if (fsm->buflen < BUFLEN)
            fsm->buffer[fsm->buflen++] = 0;
    }

    action flush {
        if (strlen(fsm->buffer) > 0) {
            char *a = expandAlias(fsm->buffer);
            char *paths[2];
            getRedirectionPaths(a, paths);
            command->redirectFromPath = paths[0];
            command->redirectToPath   = paths[1];
            expandGlob(command, a);
            command->path = command->argv[0];
            queueInsert(queue, command, (queueNodeFreeFunction) commandFree);
            free(a);
        }
    }

    action none {
        command->connectionMask = commandConnectionNone;
    }

    action seq {
        command->connectionMask = commandConnectionSequential;
    }

    action pipe {
        command->connectionMask = commandConnectionPipe;
    }

    action clear { command = commandNew(); fsm->buflen = 0; }

    pipe = "|" >pipe %flush;
    seq  = ";" >seq  %flush;
    default = space* ^(pipe|seq)+ >clear $append %term;

    main := default ((pipe|seq) default)* 0 >none $flush;
}%%

%% write data;

/* Interpret command array */
queue_t *interpret_line(char *buffer)
{
    const char *p  = buffer;
    const char *pe = buffer + strlen(buffer)+1;
    struct params *fsm = malloc(sizeof(struct params));
    queue_t *queue = queueNew();
    command_t *command = NULL;

    %% write init;
    %% write exec;

    free(fsm);
    return queue;
}

// vim: et ts=4 sw=4 sts=4
