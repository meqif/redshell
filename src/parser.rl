/* See LICENSE file for copyright and license details. */

#include <string.h>

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

    action append {
        if (fsm->buflen < BUFLEN) {
            fsm->buffer[fsm->buflen++] = fc;
            fsm->buffer[fsm->buflen]   = 0;
        }
    }

    action space {
        if (fsm->buflen < BUFLEN) {
            fsm->buffer[fsm->buflen++] = ' ';
            fsm->buffer[fsm->buflen]   = 0;
        }
    }

    action append_in {
        if (fsm->stdin_len < BUFLEN) {
            fsm->stdin[fsm->stdin_len++] = fc;
            fsm->stdin[fsm->stdin_len]   = 0;
        }
    }

    action space_in {
        if (fsm->stdin_len < BUFLEN) {
            fsm->stdin[fsm->stdin_len++] = ' ';
            fsm->stdin[fsm->stdin_len]   = 0;
        }
    }

    action append_out {
        if (fsm->stdout_len < BUFLEN) {
            fsm->stdout[fsm->stdout_len++] = fc;
            fsm->stdout[fsm->stdout_len]   = 0;
        }
    }

    action space_out {
        if (fsm->stdout_len < BUFLEN) {
            fsm->stdout[fsm->stdout_len++] = ' ';
            fsm->stdout[fsm->stdout_len]   = 0;
        }
    }

    action new {
        fsm->buflen = 0;
        fsm->stdin_len = 0;
        fsm->stdout_len = 0;
        fsm->buffer[0] = 0;
        fsm->stdout[0] = 0;
        fsm->stdin[0] = 0;
        command = commandNew();
    }

    action term {
        if (fsm->buflen > 0) {
            char *tmp = expandAlias(fsm->buffer);
            expandGlob(command, tmp);
            free(tmp);
            command->path = command->argv[0];
            if (fsm->stdin_len > 0)
                command->redirectFromPath = expand(fsm->stdin);
            if (fsm->stdout_len > 0)
                command->redirectToPath = expand(fsm->stdout);
            queueInsert(queue, command, (queueNodeFreeFunction) commandFree);
            command = NULL;
        }
    }

    action none { command->connectionMask = commandConnectionNone; }
    action pipe { command->connectionMask = commandConnectionPipe; }
    action seq  { command->connectionMask = commandConnectionSequential; }
    action bg   { command->connectionMask = commandConnectionBackground; }

    pipe    = "|" >pipe %term;
    seq     = ";" >seq  %term;
    bg      = "&" >bg   %term;
    common  = ^(0|space|";"|"|"|">"|"<"|"&");
    word    = common+ $append;
    word_in = common+ $append_in;
    word_out = common+ $append_out;
    command = word (space+ %space word)*;
    stdin   = "<" space* word_in (space+ %space_in word_in)*;
    stdout  = ">" space* word_out (space+ %space_out word_out)*;
    redir_alpha = stdin space* stdout?;
    redir_beta  = stdout space* stdin?;
    complex = space* command space* (redir_alpha|redir_beta)? space*;
    main := ( complex >new
    ( ((pipe|seq) complex >new)+ |
        '&' >bg |
        '' >none $term ) | space* )
    0 $term;
}%%

%% write data;

queue_t *parseInput(char *buffer)
{
    const char *p  = buffer;
    const char *pe = buffer + strlen(buffer)+1;
    struct params *fsm = malloc(sizeof(struct params));
    queue_t *queue = queueNew();
    command_t *command = NULL;

    fsm->buflen = 0;

    %% write init;
    %% write exec;

    if (fsm->cs == parser_error)
        fprintf(stderr, "Parser error near `%c'\n", *p);

    if (command != NULL)
        free(command);
    free(fsm);
    return queue;
}

// vim: et ts=4 sw=4 sts=4
