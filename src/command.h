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
