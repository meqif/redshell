/* See LICENSE file for copyright and license details. */

#ifndef PIPELINE_H
#define PIPELINE_H

typedef struct __pipeline_t {
    int pipes;              /* amount of command line arguments for the command */
    command_t **commands;   /* commands involved in the pipe */
    char *redirectToPath;   /* path to redirect output, or NULL */
    char *redirectFromPath; /* path from which input is redirected, or NULL */
    int bg;                 /* whether this is a background command */
} pipeline_t;

pipeline_t *pipelineNew();
void pipelineFree(pipeline_t *pipeline);

#endif /* PIPELINE_H */
