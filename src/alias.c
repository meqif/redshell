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
