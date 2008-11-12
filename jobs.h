/* Job control */

#ifndef JOBS_H
#define JOBS_H

void executioner(char **cmd);
int pipe_exec(char **argv, int n_commands, int bg, char *infile, char *outfile);

#endif /* JOBS_H */
