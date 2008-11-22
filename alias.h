/* See LICENSE file for copyright and license details. */

#ifndef ALIAS_H
#define ALIAS_H

void addAlias(char *key, char *value);
void removeAlias(char *key);
char *getAlias(char *key);
void initializeAliases();
void destroyAliases();
void traverseAliases();

#endif /* ALIAS_H */
