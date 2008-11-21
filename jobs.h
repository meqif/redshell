/* Job control */

#ifndef JOBS_H
#define JOBS_H

void executeCommand(char **cmd);
int spawnCommand(pipeline_t *pipeline);

#endif /* JOBS_H */
