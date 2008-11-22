#include <sys/types.h>
#include <sys/stat.h>

#ifndef BUILTINS_H
#define BUILTINS_H

/* Builtin commands */
int cmd_alias   (char **argv);
int cmd_cd      (char **argv);
int cmd_eco     (char **argv);
int cmd_exit    (char **argv);
int cmd_export  (char **argv);
int cmd_listar  (char **argv);
int cmd_pwd     (char **argv);
int cmd_timeout (char **argv);
int cmd_unalias (char **argv);

/* command structure */
struct cmd_struct {
	const char *cmd;
	int (*fn)(char **);
};

static const struct cmd_struct commands[] = {
    { "alias",   cmd_alias   },
    { "cd",      cmd_cd      },
    { "eco",     cmd_eco     },
    { "exit",    cmd_exit    },
    { "export",  cmd_export  },
    { "listar",  cmd_listar  },
    { "pwd",     cmd_pwd     },
    { "timeout", cmd_timeout },
    { "unalias", cmd_unalias },
};

#endif /* BUILTINS_H */
