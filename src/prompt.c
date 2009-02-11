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

#include <sys/types.h>
#include <pwd.h>

#include "common.h"
#include "prompt.h"

static char *prompt;

static char *getusername(uid_t uid)
{
    struct passwd *user = getpwuid(uid);
    char *username = user->pw_name;
    return username;
}

int setPrompt(char *format)
{
    if (format == NULL) {
        char *username = getusername(getuid());
        char hostname[BUFSIZE];
        char *tmp = malloc((BUFSIZE+1) * sizeof(char));

        gethostname(hostname, BUFSIZE);
        snprintf(tmp, BUFSIZE, "%s%s@%s > %s", RED, username, hostname, CLEAR);

        if (prompt != NULL) free(prompt);
        free(username);
        prompt = tmp;
    } else
        prompt = format;

    return 0;
}

char *getPrompt(void)
{
    return prompt;
}

// vim: et ts=4 sw=4 sts=4
