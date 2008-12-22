/* See LICENSE file for copyright and license details. */

#ifndef PROMPT_H
#define PROMPT_H

/* Shell colours */
#define RED      "\033[1;31m"
#define CLEAR    "\033[0m"

int setPrompt(char *format);
char *getPrompt(void);

#endif /* PROMPT_H */
