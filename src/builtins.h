#include <sys/types.h>
#include <sys/stat.h>

#ifndef BUILTINS_H
#define BUILTINS_H

/*!
 \defgroup builtins Builtin commands
 \addtogroup builtins
 \{
 */

/* Builtin commands */

/*! \brief Create an alias or list all aliases */
int cmd_alias   (char **argv);

/*! \brief Change the current directory */
int cmd_cd      (char **argv);

/*! \brief Echo the user input one word per
 * line */
int cmd_eco     (char **argv);

/*! \brief Exit the shell */
int cmd_exit    (char **argv);

/*! \brief Export an environment variable */
int cmd_export  (char **argv);

/*! \brief List the contents of a directory */
int cmd_listar  (char **argv);

/*! \brief Print the current working directory */
int cmd_pwd     (char **argv);

/*! \brief Run a command for at most n seconds */
int cmd_timeout (char **argv);

/*! \brief Remove an alias */
int cmd_unalias (char **argv);

/*! \brief Represents an entry with the builtin command name and its function */
struct cmd_struct {
    /*! \brief Name of the command */
    const char *cmd;
    /*! \brief Address of the function */
    int (*fn)(char **);
};

/*! \brief Array with all known builtin commands */
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

/*! \} */

#endif /* BUILTINS_H */
