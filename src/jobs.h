/* Job control */

#ifndef JOBS_H
#define JOBS_H

/*!
 \addtogroup jobs
 \{
 */

/*! \brief Execute a single command */
int executeCommand(command_t *cmd);

/*! \brief Execute a pipeline of commands */
int spawnCommand(pipeline_t *pipeline);

/*! \} */

#endif /* JOBS_H */
