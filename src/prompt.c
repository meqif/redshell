/* See LICENSE file for copyright and license details. */

#include <sys/types.h>

#include "common.h"
#include "helper.h"
#include "prompt.h"

static char *prompt;

int setPrompt(char *format)
{
    if (format == NULL) {
        char *username = getusername(getuid());
        char hostname[BUFSIZE];
        char *tmp = malloc((BUFSIZE+1) * sizeof(char));

        gethostname(hostname, BUFSIZE);
        snprintf(tmp, BUFSIZE, "%s%s@%s > %s", RED, username, hostname, CLEAR);

        free(prompt);
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
