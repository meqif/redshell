/* Common macro definitions and includes */

#ifndef COMMON_H
#define COMMON_H

/* Common includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "command.h"
#include "pipeline.h"

/* Common macro definitions */
#define DELIMITERS " \t\n"
#define BUF_SIZE 1000
#define HIST_SIZE 100
#define PERM_MASK 07777

#endif /* COMMON_H */
