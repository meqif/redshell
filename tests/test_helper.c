/* See LICENSE file for copyright and license details. */

#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmockery.h>
#include <unistd.h>

#include "common.h"
#include "helper.h"

void testStringCount(void **state)
{
    char *str = "/usr/bin/gcc";
    assert_int_equal(strstrcnt(str, '/'), 3);
}

// vim: et ts=4 sw=4 sts=4
