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

#include <string.h>

#include "command.h"
#include "common.h"
#include "helper.h"
#include "queue.h"

struct params
{
    char buffer[BUFSIZE+1];
    char stdin[BUFSIZE+1];
    char stdout[BUFSIZE+1];
    int buflen;
    int stdin_len;
    int stdout_len;
    int cs;
};

%%{
    machine parser;
    access fsm->;

    action append {
        if (fsm->buflen < BUFSIZE) {
            fsm->buffer[fsm->buflen++] = fc;
            fsm->buffer[fsm->buflen]   = 0;
        }
    }

    action append_in {
        if (fsm->stdin_len < BUFSIZE) {
            fsm->stdin[fsm->stdin_len++] = fc;
            fsm->stdin[fsm->stdin_len]   = 0;
        }
    }

    action append_out {
        if (fsm->stdout_len < BUFSIZE) {
            fsm->stdout[fsm->stdout_len++] = fc;
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
            if (expandGlob(command, tmp) != 0) {
                fprintf(stderr, "Illegal character found, aborting. Blame wordexp.\n");
                free(tmp);
                fbreak;
            }
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
    action bg   { command->connectionMask = commandConnectionBackground; }
    action seq  {
        if (command->connectionMask != commandConnectionBackground)
            command->connectionMask = commandConnectionSequential;
    }

    pipe    = "|" >pipe %term;
    seq     = ";" >seq  %term;
    bg      = "&" >bg;
    word    = ^(0|space|";"|"|"|">"|"<"|"&")+;
    string  = word (space+ word)*;
    command = string $append;
    stdin   = "<" space* string $append_in;
    stdout  = ">" space* string $append_out;
    redir_alpha = stdin space* stdout?;
    redir_beta  = stdout space* stdin?;
    complex = (space* command space* (redir_alpha|redir_beta)? space* bg? space*) >new;
    main := ( complex ( ((pipe|seq) complex)+ | '' $term ) | space* )
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

    if (fsm->cs == parser_error) {
        fprintf(stderr, "Parser error near `%c%c'\n", *(p-1), *p);
        queueFree(queue);
        queue = queueNew();
    }

    if (command != NULL)
        free(command);
    free(fsm);
    return queue;
}

// vim: et ts=4 sw=4 sts=4
