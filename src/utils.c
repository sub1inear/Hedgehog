#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#include "mem.h"
#include "msg.h"
#include "utils.h"

// #define HHG_DEBUG_SYSTEM

FILE *hhg_utils_fopen(const char *filename, const char *mode)
{
    FILE *file = fopen(filename, mode);
    if (file == NULL)
        hhg_fatal_error(
            "opening %s: %s",
            filename,
            strerror(errno)
        );
    return file;
}

int64_t hhg_utils_str_to_int64(const char *str)
{
    bool negative = false;
    if (*str == '-') {
        negative = true;
        str++;
    }
    int64_t result = 0;
    while (*str) {
        if (*str < '0' || *str > '9')
            hhg_fatal_error("invalid integer: %s", str);
        if (result > (INT64_MAX - (*str - '0')) / 10)
            hhg_fatal_error("integer overflow: %s", str);
        result = result * 10 + (*str - '0');
        str++;
    }
    return negative ? -result : result;
}

int hhg_utils_system(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

#ifdef HHG_DEBUG_SYSTEM
    vprintf(fmt, args);
    va_end(args);
    return 0;
#else
    int len = vsnprintf(NULL, 0, fmt, args);
    if (len < 0)
        hhg_compiler_error("vsnprintf failed");
    
    char *cmd = hhg_malloc(len + 1);
    int vsnprintf_result = vsnprintf(cmd, len + 1, fmt, args);
    hhg_assert(vsnprintf_result == len);
    
    va_end(args);

    return system(cmd);
#endif
}

void hhg_utils_assert(const char *expr_str, const char *file, int line)
{
    hhg_compiler_error(
        "assertion failed: %s, at %s:%i",
        expr_str,
        file,
        line
    );
}
