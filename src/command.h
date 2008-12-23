/* See LICENSE file for copyright and license details. */

#ifndef COMMAND_H
#define COMMAND_H

/*!
 \addtogroup command
 \{
 */

enum {
    commandConnectionNone       = 0,
    commandConnectionPipe       = 1,
    commandConnectionBackground = 2,
    commandConnectionSequential = 3,
};

/*! \brief Represents a command and all relevant information */
typedef struct __command_t {
    /*! \brief Path of the command */
    char *path;
    /*! \brief Number of command line arguments for the command */
    int argc;
    /*! \brief Arguments provided for the command */
    char **argv;
    /*! \brief Path to redirect output, or NULL */
    char *redirectToPath;
    /*! \brief Path from which input is redirected, or NULL */
    char *redirectFromPath;

    int connectionMask;
} command_t;

/*! \brief Create new command */
command_t *commandNew(void);

/*! \brief Remove a command */
void commandFree(command_t *command);

/*! \} */

#endif /* COMMAND_H */
