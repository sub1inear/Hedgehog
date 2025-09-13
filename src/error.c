#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "error.h"

void hhg_fatal_error(const char *str, ...) {
    va_list va;
    va_start(va, str);
    
    fputs("fatal error: ", stderr);
    vfprintf(stderr, str, va);
    putchar('\n');

    va_end(va);
    exit(1);
}