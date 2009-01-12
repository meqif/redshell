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
    char stdin[BUFLEN+1];
    char stdout[BUFLEN+1];
    int buflen;
    int stdin_len;
    int stdout_len;
    int cs;
};

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
            if (fsm->stdin_len != 0)
                command->redirectFromPath = strdup(fsm->stdin);
            if (fsm->stdout_len != 0)
                command->redirectToPath = strdup(fsm->stdout);
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

    action clear {
        command = commandNew();
        fsm->buflen = 0;
        fsm->stdin_len = 0;
        fsm-> stdout_len = 0;
    }

    action error { fprintf(stderr, "Parse error.\n"); }

    action stdin  {
        if (fsm->stdin_len < BUFLEN)
            fsm->stdin[fsm->stdin_len++] = fc;
    }

    action stdout {
        if (fsm->stdout_len < BUFLEN)
            fsm->stdout[fsm->stdout_len++] = fc;
    }

    pipe = "|" >pipe %flush;
    seq  = ";" >seq  %flush;
    stdin   = space* ^(0|">"|"<"|pipe|seq|space)+ $stdin;
    stdout  = space* ^(0|">"|"<"|pipe|seq|space)+ $stdout;
    default = space* ^(0|">"|"<"|pipe|seq|space)+ >clear $append %term;

    main := default space* ("<" stdin)? space* (">" stdout)? space* ((pipe|seq) default)* 0 >none $flush;
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
