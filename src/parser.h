/* See LICENSE file for copyright and license details. */

#ifndef PARSER_H
#define PARSER_H

/*!
 \addtogroup parser
 \{
 */

#include "queue.h"

char *getRedirectionPaths(char *line, char *paths[]);

/*! \brief Interpret user input */
queue_t *interpret_line(char *buffer);

/*! \} */

#endif /* PARSER_H */
