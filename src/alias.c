/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include <string.h>
#include "alias.h"
#include "common.h"

static GTree *tree;

alias_t *aliasNew()
{
    alias_t *alias = NULL;
    alias = malloc(sizeof(*alias));
    if(alias == NULL) {
        return NULL;
    }
    alias->value = NULL;
    alias->in_use = 0;
    return alias;
}

void aliasFree(gpointer data)
{
    alias_t *alias = data;
    assert(alias != NULL);
    free(alias->value);
    free(alias);
}

static gint compare(gconstpointer a, gconstpointer b, gpointer data)
{
    (void) data; /* avoid warning about unused parameter */
    return strcmp(a,b);
}

void addAlias(char *key, char *value)
{
    alias_t *alias = aliasNew();
    alias->value = value;
    g_tree_insert(tree, key, alias);
}

void removeAlias(char *key)
{
    g_tree_remove(tree, key);
}

char *getAlias(char *key)
{
    alias_t *alias = g_tree_lookup(tree, key);
    if (alias != NULL && alias->in_use == 0) {
        alias->in_use = 1;
        return alias->value;
    }
    return NULL;
}

static gboolean releaseAlias(gpointer key, gpointer value, gpointer data)
{
    (void) key;  /* avoid warning about unused parameter */
    (void) data; /* avoid warning about unused parameters */
    ((alias_t *)value)->in_use = 0;
    return FALSE;
}

void releaseAliases()
{
    g_tree_foreach(tree, releaseAlias, NULL);
}

void initializeAliases()
{
    tree = g_tree_new_full(compare, NULL, g_free, aliasFree);
}

void destroyAliases()
{
    g_tree_destroy(tree);
    tree = NULL;
}

static gboolean printAliases(gpointer key, gpointer value, gpointer data)
{
    (void) data; /* avoid warning about unused parameter */
    alias_t *alias = value;
    printf("%s=%s\n", (char *)key, alias->value);
    return FALSE;
}

void traverseAliases()
{
    g_tree_foreach(tree, printAliases, NULL);
}

// vim: et ts=4 sw=4 sts=4
