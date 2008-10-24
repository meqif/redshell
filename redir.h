/* See LICENSE file for copyright and license details. */

int add_pid(pid_t new_pid);
void hellspawn(char **cmd);
int external_exec(char **myArgv, int bg);
int join(char **argv, int count, int bg);
