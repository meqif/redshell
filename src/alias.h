/* See LICENSE file for copyright and license details. */

#ifndef ALIAS_H
#define ALIAS_H

/*!
 \addtogroup alias
 \{
 */

#include <glib.h>

/*! \brief Represents an alias and all relevant information */
typedef struct __alias_t {
    /*! \brief The value of the alias */
    char *value;
    /*! \brief Whether the alias is in use (for command expansion) */
    int in_use;
} alias_t;

/*!
 * \brief Create new alias.
 */
alias_t *aliasNew(void);

/*!
 * \brief Free and destroy an alias.
 */
void aliasFree(gpointer data);

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
 * \brief Remove an alias.
 */
void initializeAliases(void);

/*!
 * \brief Free and destroy alias dictionary.
 */
void destroyAliases(void);

/*!
 * \brief Print all aliases.
 */
void traverseAliases(void);

/*! \} */

#endif /* ALIAS_H */
