#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef BUILTINS_H
#define BUILTINS_H

/* Builtin commands */
void print_file_info(char *name, char *path, struct stat s);
int cmd_cd(char **argv);
int cmd_eco(char **myArgv);
int cmd_exit(char **argv);
int cmd_listar(char **path);
int cmd_pwd(char **argv);

#endif /* BUILTINS_H */
