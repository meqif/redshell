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

char *getRedirectionPaths(char *line, char *paths[])
{
    int i;
    int len = strlen(line);
    int seen_letter = 0;
    int seen_letter_in = 0;
    char *start_out = NULL;
    char *end_out = NULL;
    char *start_in = NULL;
    char *end_in = NULL;
    char *redir_out = NULL;
    char *redir_in = NULL;
    for (i = 0; i < len; line++, i++) {
        if (*line == '>') {
            *line = '\0';
            start_out = line+1;
        }
        else if (*line == '<') {
            *line = '\0';
            start_in = line+1;
        }
        else if (start_out != NULL && seen_letter == 0 && end_out == NULL) {
            while(*line == ' ') {
                start_out = ++line;
                i++;
            }
            seen_letter = 1;
        }
        else if (start_in != NULL && seen_letter_in == 0 && end_in == NULL) {
            while (*line == ' ') {
                start_in = ++line;
                i++;
            }
            seen_letter_in = 1;
        }
        if (start_out != NULL && end_out == NULL && seen_letter == 1 && *line == ' ') {
            end_out = line;
            seen_letter = 0;
        }
        if (start_in != NULL && end_in == NULL && seen_letter_in == 1 && *line == ' ') {
            end_in = line;
            seen_letter_in = 1;
        }
    }
    if (start_out != NULL) {
        if (end_out == NULL)
            end_out = line;
        redir_out = calloc(end_out-start_out+1, sizeof(char));
        strncpy(redir_out, start_out, end_out-start_out);
    }
    if (start_in != NULL) {
        if (end_in == NULL)
            end_in = line;
        redir_in = calloc(end_in-start_in+1, sizeof(char));
        strncpy(redir_in, start_in, end_in-start_in);
    }
    paths[0] = redir_in;
    paths[1] = redir_out;
    return redir_out;
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
        if (strlen(fsm->buffer) > 0) {
            char *a = expandAlias(fsm->buffer);
            command_t *command = commandNew();
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

    action clear { fsm->buflen = 0; }

    pipe = "|";
    seq  = ";";
    default = space* ^(pipe|seq)+ >clear $append %term;

    main := default ((pipe|seq) default)* 0;
}%%

%% write data;

/* Interpret command array */
queue_t *interpret_line(char *buffer)
{
    const char *p  = buffer;
    const char *pe = buffer + strlen(buffer)+1;
    struct params *fsm = malloc(sizeof(struct params));
    queue_t *queue = queueNew();

    %% write init;
    %% write exec;

    return queue;
}

// vim: et ts=4 sw=4 sts=4
