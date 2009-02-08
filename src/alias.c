/* See LICENSE file for copyright and license details. */

#include <assert.h>
#include <string.h>
#include "alias.h"
#include "common.h"

static HashTable *table;

static alias_t *aliasNew()
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

static void aliasFree(void* data)
{
    alias_t *alias = data;
    assert(alias != NULL);
    free(alias->value);
    free(alias);
}

void addAlias(char *key, char *value)
{
    alias_t *alias = aliasNew();
    alias->value = value;
    hash_table_insert(table, key, alias);
}

void removeAlias(char *key)
{
    hash_table_remove(table, key);
}

char *getAlias(char *key)
{
    alias_t *alias = hash_table_lookup(table, key);
    if (alias != NULL && alias->in_use == 0) {
        alias->in_use = 1;
        return alias->value;
    }
    return NULL;
}

void releaseAliases()
{
    HashTableIterator iterator;
    hash_table_iterate(table, &iterator);

    while (hash_table_iter_has_more(&iterator)) {
        alias_t *alias = hash_table_iter_next(&iterator);
        assert(alias != NULL);
        alias->in_use = 0;
    }
}

void initializeAliases()
{
    table = hash_table_new(string_hash, string_equal);
    hash_table_register_free_functions(table, free, aliasFree);
}

void destroyAliases()
{
    hash_table_free(table);
    table = NULL;
}

// vim: et ts=4 sw=4 sts=4
