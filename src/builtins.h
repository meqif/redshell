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

/*! \brief Echo the user input */
int cmd_echo     (char **argv);

/*! \brief Exit the shell */
int cmd_exit    (char **argv);

/*! \brief Export an environment variable */
int cmd_export  (char **argv);

/*! \brief Print the current working directory */
int cmd_pwd     (char **argv);

/*! \brief Remove an alias */
int cmd_unalias (char **argv);

/*! \brief Check if a given command is builtin */
int isBuiltin(char *cmd);

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
    { "echo",    cmd_echo    },
    { "exit",    cmd_exit    },
    { "export",  cmd_export  },
    { "pwd",     cmd_pwd     },
    { "unalias", cmd_unalias },
};

/*! \} */

#endif /* BUILTINS_H */
