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

#ifndef ALIAS_H
#define ALIAS_H

/*!
 \addtogroup alias
 \{
 */

#include "hash-table.h"
#include "hash-string.h"
#include "compare-string.h"

/*! \brief Represents an alias and all relevant information */
typedef struct __alias_t {
    /*! \brief The value of the alias */
    char *value;
    /*! \brief Whether the alias is in use (for command expansion) */
    int in_use;
} alias_t;

/*!
 * \brief Add an alias.
 */
void addAlias(char *key, char *value);

/*!
 * \brief Remove an alias.
 */
void removeAlias(char *key);

/*!
 * \brief Retrieve the value of an alias
 */
char *getAlias(char *key);

/*!
 * \brief Release aliases (mark them as not in use).
 */
void releaseAliases(void);

/*!
 * \brief Print all known key-alias pairs
 */
void printAliases(void);

/*!
 * \brief Remove an alias.
 */
void initializeAliases(void);

/*!
 * \brief Free and destroy alias dictionary.
 */
void destroyAliases(void);

/*! \} */

#endif /* ALIAS_H */
