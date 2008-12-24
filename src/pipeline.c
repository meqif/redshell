/* See LICENSE file for copyright and license details. */

#include <assert.h>

#include "command.h"
#include "common.h"
#include "pipeline.h"

pipeline_t *pipelineNew(void)
{
    pipeline_t *pipeline = NULL;
    pipeline = malloc(sizeof(*pipeline));
    if(pipeline == NULL) {
        return NULL;
    }
    pipeline->pipes = 0;
    pipeline->commands = NULL;
    pipeline->redirectToPath = NULL;
    pipeline->redirectFromPath = NULL;
    pipeline->bg = 0;

    return pipeline;
}

void pipelineFree(pipeline_t *pipeline)
{
    int i;
    assert(pipeline != NULL);

    for (i = 0; i < pipeline->pipes; i++) {
        commandFree(pipeline->commands[i]);
        pipeline->commands[i] = NULL;
    }

    /* path is an alias for argv[0], which has already been freed */
    if(pipeline->redirectToPath != NULL) {
        free(pipeline->redirectToPath);
        pipeline->redirectToPath = NULL;
    }
    if(pipeline->redirectFromPath != NULL) {
        free(pipeline->redirectFromPath);
        pipeline->redirectFromPath = NULL;
    }
    free(pipeline);
}

// vim: et ts=4 sw=4 sts=4
