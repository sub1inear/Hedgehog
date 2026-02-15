#ifndef HHG_UTILS_H
#define HHG_UTILS_H

#include <stdint.h>
#include <stdio.h>

#define HHG_ARR_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define HHG_UNUSED(...) (void)(__VA_ARGS__)

#define HHG_ANSI_COLOR_CLEAR "\x1b[0m"
#define HHG_ANSI_COLOR_RED "\x1b[1;31m"
#define HHG_ANSI_COLOR_YELLOW "\x1b[1;33m"

// safe fopen of a file
// crashes with hhg_fatal_error if file cannot be opened
FILE *hhg_utils_fopen(const char *filename, const char *mode);

// truncate path to filename, separated by / or \
// returns path if no separator found
char *hhg_utils_path_trunc(char *path);
inline const char *hhg_utils_path_trunc_const(const char *path) {
    return hhg_utils_path_trunc((char *)path);
}

int64_t hhg_utils_str_to_int64(const char *str);

#endif
