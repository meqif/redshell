/* See LICENSE file for copyright and license details. */

#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmockery.h>
#include <unistd.h>
#include <string.h>

#include "alias.h"
#include "common.h"
#include "helper.h"

void testInitializeAliases(void **state)
{
    initializeAliases();
}

void testAddAlias(void **state)
{
    char *name = calloc(3, sizeof(char));
    strcpy(name, "ls");
    char *content = calloc(11, sizeof(char));
    strcpy(content, "ls --color");
    addAlias(name, content);
    assert_string_equal(getAlias("ls"), "ls --color");

    char *name2 = calloc(3, sizeof(char));
    strcpy(name2, "ll");
    char *content2 = calloc(7, sizeof(char));
    strcpy(content2, "ls -lh");
    addAlias(name2, content2);
    assert_string_equal(getAlias("ll"), "ls -lh");
}

void testAlias(void **state)
{
    char *name = calloc(3, sizeof(char));
    strcpy(name, "ls");
    char *content = calloc(11, sizeof(char));
    strcpy(content, "ls --color");
    addAlias(name, content);

    char *name2 = calloc(3, sizeof(char));
    strcpy(name2, "ll");
    char *content2 = calloc(7, sizeof(char));
    strcpy(content2, "ls -lh");
    addAlias(name2, content2);

    char *name3 = calloc(4, sizeof(char));
    strcpy(name3, "lld");
    char *content3 = calloc(15, sizeof(char));
    strcpy(content3, "ll --sort=date");
    addAlias(name3, content3);

    char *command = expandAlias("ll");
    assert_string_equal(command, "ls --color -lh");
    free(command);
    releaseAliases();

    command = expandAlias("ll $HOME");
    assert_string_equal(command, "ls --color -lh $HOME");
    free(command);
    releaseAliases();

    command = expandAlias("lld");
    assert_string_equal(command, "ls --color -lh --sort=date");
    free(command);
    releaseAliases();
}

void testDestroyAliases(void **state)
{
    destroyAliases();
}

// vim: et ts=4 sw=4 sts=4
