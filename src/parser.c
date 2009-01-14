/* See LICENSE file for copyright and license details. */

#include <string.h>

#include "alias.h"
#include "command.h"
#include "common.h"
#include "helper.h"
#include "queue.h"

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

/* Interpret command array */
queue_t *interpret_line(char *buffer)
{
    queue_t *queue = queueNew();

    char *ptr = buffer;
    char *ptr_orig = buffer;
    do {
        if (*ptr_orig == '\n' || *ptr_orig == '|' ||
            *ptr_orig == ';'  || *ptr_orig == '\0' || *ptr_orig == '&') {
            command_t *command = commandNew();
            switch(*ptr_orig) {
                case '|':
                    command->connectionMask = commandConnectionPipe;
                    break;
                case ';':
                    command->connectionMask = commandConnectionSequential;
                    break;
                case '&':
                    *ptr_orig = '\0'; /* work around this later */
                    command->connectionMask = commandConnectionBackground;
                    break;
                default:
                    command->connectionMask = commandConnectionNone;
            }
            char *tmp = calloc((size_t) (ptr_orig-ptr+1), sizeof(char));
            strncpy(tmp, ptr, ptr_orig-ptr);
            char *a = expandAlias(tmp);
            releaseAliases();
            char *paths[2];
            getRedirectionPaths(a, paths);
            command->redirectFromPath = paths[0];
            command->redirectToPath   = paths[1];
            expandGlob(command, a);
            command->path = command->argv[0];
            free(a);
            free(tmp);
            ptr = ptr_orig+1;
            queueInsert(queue, command, (queueNodeFreeFunction) commandFree);
        }
    } while (*ptr_orig++ != 0);

    return queue;
}

// vim: et ts=4 sw=4 sts=4
