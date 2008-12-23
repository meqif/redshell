/* See LICENSE file for copyright and license details. */

#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmockery.h>
#include <unistd.h>

#include "common.h"
#include "builtins.h"

void testIsBuiltin(void **state)
{
    assert_int_equal(isBuiltin("cd"), 1);
    assert_int_equal(isBuiltin("external_command"), 0);
}

// vim: et ts=4 sw=4 sts=4
