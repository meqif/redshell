/* See LICENSE file for copyright and license details. */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmockery.h>

#include "test_alias.h"
#include "test_builtins.h"
#include "test_helper.h"

int main()
{
    const UnitTest tests[] = {
        unit_test(testInitializeAliases),
        unit_test(testAddAlias),
        unit_test(testAlias),
        unit_test(testDestroyAliases),
        unit_test(testStringCount),
        unit_test(testIsBuiltin),
    };
    return run_tests(tests);
}

// vim: et ts=4 sw=4 sts=4
