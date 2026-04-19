#ifndef HHG_UTILS_H
#define HHG_UTILS_H

#include <stdint.h>
#include <stdio.h>

#define HHG_ARR_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define HHG_UNUSED(...) (void)(__VA_ARGS__)

#define HHG_ANSI_COLOR_CLEAR "\x1b[0m"
#define HHG_ANSI_COLOR_RED "\x1b[1;31m"
#define HHG_ANSI_COLOR_YELLOW "\x1b[1;33m"

#define hhg_assert(expr) if (!(expr)) hhg_utils_assert(#expr, __FILE__, __LINE__)

// safe fopen of a file
// crashes with hhg_fatal_error if file cannot be opened
FILE *hhg_utils_fopen(const char *filename, const char *mode);

int64_t hhg_utils_str_to_int64(const char *str);

// calls system with a format string
int hhg_utils_system(const char *format, ...);

int hhg_utils_exec_file(const char *filename);

// internal function for hhg_assert, not meant to be called directly
void hhg_utils_assert(const char *expr_str, const char *file, int line);

#endif
