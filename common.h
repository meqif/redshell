/* Common macro definitions and includes */

#ifndef COMMON_H
#define COMMON_H

/* Common macro definitions */
#define DELIMITERS " \t\n"
#define BUF_SIZE 1000
#define HIST_SIZE 100
#define PERM_MASK 07777

typedef struct __command_t {
    char *path;             /* path of the command */
    int argc;               /* amount of command line arguments for the command */
    char **argv;            /* arguments provided for the command */
    char *redirectToPath;   /* path to redirect output, or NULL */
    char *redirectFromPath; /* path from which input is redirected, or NULL */
    int bg;                 /* whether this is a background command */
} command_t;

typedef struct __pipeline_t {
    int pipes;              /* amount of command line arguments for the command */
    command_t **commands;   /* commands involved in the pipe */
    char *redirectToPath;   /* path to redirect output, or NULL */
    char *redirectFromPath; /* path from which input is redirected, or NULL */
    int bg;                 /* whether this is a background command */
} pipeline_t;

/* Common includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#endif /* COMMON_H */
