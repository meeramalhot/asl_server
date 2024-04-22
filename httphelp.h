#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function prototypes
FILE *get_file_descriptor(const char *file_name);
long int get_file_size(FILE *f);
const char *get_mime(char *file);
const char *format_time(void);

#endif /* FILE_UTILS_H */