/* Job control */

#ifndef JOBS_H
#define JOBS_H

/*!
 \addtogroup jobs
 \{
 */

#include "queue.h"

/*! \brief Execute a single command */
int executeCommand(command_t *cmd);

/* Execute a queue of commands */
int executeCommandsInQueue(queue_t *commandQueue);

/*! \} */

#endif /* JOBS_H */
