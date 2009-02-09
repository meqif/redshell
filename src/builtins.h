/*
 * Copyright (c) 2008-2009 Ricardo Martins <ricardo@scarybox.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

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
