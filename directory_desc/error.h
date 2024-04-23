#ifndef ERRORS_H
#define ERRORS_H

#include "apue.h"
#include <errno.h>
#include <stdarg.h>

void err_ret(const char *fmt, ...);
void err_sys(const char *fmt, ...);
void err_cont(int error, const char *fmt, ...);
void err_exit(int error, const char *fmt, ...);
void err_dump(const char *fmt, ...);
void err_msg(const char *fmt, ...);
void err_quit(const char *fmt, ...);

#endif /* ERRORS_H */
