#ifndef PATH_ALLOC_H
#define PATH_ALLOC_H

#include "apue.h"
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>

/* If PATH_MAX is indeterminate, no guarantee this is adequate */
#define PATH_MAX_GUESS 1024

/* Function declaration */
char *path_alloc(size_t *sizep);

#endif /* PATH_ALLOC_H */
