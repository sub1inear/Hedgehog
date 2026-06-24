#ifndef HHG_UTILS_H
#define HHG_UTILS_H

#include <stdint.h>
#include <stdio.h>

#define HHG_ARR_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define HHG_UNUSED(...) (void)(__VA_ARGS__)

#define HHG_ANSI_COLOR_CLEAR "\x1b[0m"
#define HHG_ANSI_COLOR_RED "\x1b[1;31m"
#define HHG_ANSI_COLOR_YELLOW "\x1b[1;33m"

#ifdef _WIN32
#define HHG_WINDOWS
#elif defined(__linux__)
#define HHG_LINUX
#elif defined(__APPLE__) && defined(__MACH__)
#define HHG_MACOS
#else
#error "unsupported OS"
#endif

#if defined(HHG_LINUX) || defined(HHG_MACOS)
#define HHG_POSIX
#endif

#ifdef HHG_WINDOWS
#define HHG_EXEC_EXT ".exe"
#elif defined(HHG_UTILS_POSIX)
#define HHG_EXEC_EXT ""
#endif

#define hhg_assert(expr) if (!(expr)) hhg_utils_assert(#expr, __FILE__, __LINE__)

typedef struct hhg_str hhg_str_t;

// safe fopen of a file
// crashes with hhg_fatal_error if file cannot be opened
FILE *hhg_utils_fopen(const char *filename, const char *mode);

// safe path join of two paths
// crashes with hhg_fatal_error
// if the result does not fit in the buffer
// or format fails
void hhg_utils_join_path(
    char *buf,
    size_t size,
    const char *left,
    const char *right
);

int64_t hhg_utils_str_to_int64(const char *str);

// spawns a new process with argv (NULL-terminated array of strings)
// waits until completion and returns the exit code of process
// if stdouterr is NULL, inherits the stdout and stderr
// otherwise captures them in stdouterr (which the caller must free)
int hhg_utils_spawn(const char **argv, hhg_str_t *stdouterr);

// internal function for hhg_assert, not meant to be called directly
void hhg_utils_assert(const char *expr_str, const char *file, int line);

#endif
