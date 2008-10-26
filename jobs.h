/* Job control */

#ifndef JOBS_H
#define JOBS_H

int external_exec(char **myArgv, int bg);
int run_command(char *buffer, int bg);

#endif /* JOBS_H */
