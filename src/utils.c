#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "msg.h"

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
    while (*str++) {
        if (*str < '0' || *str > '9')
            hhg_fatal_error("invalid integer: %s", str);
        if (result > (INT64_MAX - (*str - '0')) / 10)
            hhg_fatal_error("integer overflow: %s", str);
        result = result * 10 + (*str - '0');
    }
    return negative ? -result : result;
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
