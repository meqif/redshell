/* See LICENSE file for copyright and license details. */

#ifndef ALIAS_H
#define ALIAS_H

#include <glib.h>

typedef struct __alias_t {
    char *value;
    int in_use;
} alias_t;

void addAlias(char *key, char *value);
void removeAlias(char *key);
char *getAlias(char *key);
void releaseAliases();
void initializeAliases();
void destroyAliases();
void traverseAliases();

#endif /* ALIAS_H */
