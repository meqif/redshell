/* Job control */

#ifndef JOBS_H
#define JOBS_H

void executioner(char **cmd);
int pipe_exec(char **argv, command_t *command);

#endif /* JOBS_H */
