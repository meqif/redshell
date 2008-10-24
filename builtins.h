#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef BUILTINS_H
#define BUILTINS_H

/* Builtin commands */
int cmd_cd(char **argv);
int cmd_eco(char **myArgv);
int cmd_exit(char **argv);
int cmd_listar(char **path);
int cmd_pwd(char **argv);

/* command structure */
struct cmd_struct {
	const char *cmd;
	int (*fn)(char **);
};

/* Inspired by git source code */
static const struct cmd_struct commands[] = {
    { "cd",     cmd_cd     },
    { "eco",    cmd_eco    },
    { "exit",   cmd_exit   },
    { "listar", cmd_listar },
    { "pwd",    cmd_pwd    },
};

#endif /* BUILTINS_H */
