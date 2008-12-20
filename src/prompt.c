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
        char hostname[100];
        char *tmp = malloc(200 * sizeof(char));

        gethostname(hostname, 100);
        snprintf(tmp, 200, "%s%s@%s > %s", RED, username, hostname, CLEAR);

        free(prompt);
        prompt = malloc(strlen(tmp)+1);
        strcpy(prompt, tmp);
        free(tmp);
    } else {
        prompt = format;
    }

    return 0;
}

char *getPrompt()
{
    return prompt;
}

// vim: et ts=4 sw=4 sts=4
