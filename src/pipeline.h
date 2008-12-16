/* See LICENSE file for copyright and license details. */

#ifndef PIPELINE_H
#define PIPELINE_H

/*!
 \addtogroup pipeline
 \{
 */

/*! \brief Represents a series of commands in a pipeline */
typedef struct __pipeline_t {
    /*! \brief Number of commands in the pipeline */
    int pipes;
    /*! \brief Array of commands involved in the pipe */
    command_t **commands;
    /*! \brief Path to redirect output to, or NULL */
    char *redirectToPath;
    /*! \brief Path from which input is redirected, or NULL */
    char *redirectFromPath;
    /*! \brief Whether this should run in background */
    int bg;
} pipeline_t;

/*! \brief Create a new pipeline */
pipeline_t *pipelineNew(void);

/*! \brief Free and destroy a pipeline */
void pipelineFree(pipeline_t *pipeline);

/*! \} */

#endif /* PIPELINE_H */
