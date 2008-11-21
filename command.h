/* See LICENSE file for copyright and license details. */

#ifndef COMMAND_H
#define COMMAND_H

typedef struct __command_t {
    char *path;             /* path of the command */
    int argc;               /* amount of command line arguments for the command */
    char **argv;            /* arguments provided for the command */
    char *redirectToPath;   /* path to redirect output, or NULL */
    char *redirectFromPath; /* path from which input is redirected, or NULL */
    int bg;                 /* whether this is a background command */
} command_t;

command_t *commandNew(void);
void commandFree(command_t *command);

#endif /* COMMAND_H */
