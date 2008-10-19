#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef BUILTINS_H
#define BUILTINS_H

/* Builtin commands */
void print_file_info(char *name, char *path, struct stat s);
int listar(char *path);
int eco(char **myArgv);
int pwd();
int cd(char *path);

#endif /* BUILTINS_H */
