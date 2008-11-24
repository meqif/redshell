/* See LICENSE file for copyright and license details. */

#include <glib.h>
#include <string.h>
#include "common.h"

static GTree *tree;

static gint compare(gconstpointer a, gconstpointer b, gpointer data)
{
    return strcmp(a,b);
}

void addAlias(char *key, char *value)
{
    g_tree_insert(tree, key, value);
}

void removeAlias(char *key)
{
    g_tree_remove(tree, key);
}

char *getAlias(char *key)
{
    return g_tree_lookup(tree, key);
}

void initializeAliases()
{
    tree = g_tree_new_full(compare, NULL, g_free, g_free);
}

void destroyAliases()
{
    g_tree_destroy(tree);
    tree = NULL;
}

static gboolean printAliases(gpointer key, gpointer value, gpointer data)
{
    printf("%s=%s\n", (char *)key, (char *)value);
    return FALSE;
}

void traverseAliases()
{
    g_tree_foreach(tree, printAliases, NULL);
}

// vim: et ts=4 sw=4 sts=4
