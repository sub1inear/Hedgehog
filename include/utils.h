#ifndef HHG_H
#define HHG_H

#include <stdint.h>
#include <stdio.h>

#define HHG_ARR_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))
#define HHG_STR_LEN(str) (HHG_ARR_LEN(str) - 1)
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
#error Unsupported OS.
#endif

#if defined(HHG_LINUX) || defined(HHG_MACOS)
#define HHG_POSIX
#endif

#define hhg_assert(expr) if (!(expr)) hhg_assert_core(#expr, __FILE__, __LINE__)

typedef struct hhg_stream {
    void *arg;
    void (*out_str)(void *arg, const char *str);
    void (*out_char)(void *arg, char c);
} hhg_stream_t;

typedef struct hhg_str hhg_str_t;
typedef struct arena hhg_arena_t;

// safe fopen of a file
// crashes with hhg_fatal_error if file cannot be opened
FILE *hhg_fopen(const char *filename, const char *mode);

// safe path join of two paths
// crashes with hhg_fatal_error
// if the result does not fit in the buffer
// or format fails
void hhg_join_path(
    char *buf,
    size_t size,
    const char *left,
    const char *right
);

int64_t hhg_str_to_int64(const char *str);

// spawns a new process with argv (NULL-terminated array of strings)
// waits until completion and returns the exit code of process
// if stdouterr is NULL, inherits the stdout and stderr
// otherwise captures them in stdouterr (which the caller must free)
int hhg_spawn(const char **argv, hhg_str_t *stdouterr);

#ifdef HHG_WINDOWS
// spawns a new process with a command line string
// only for cl.exe location extraction
int hhg_spawn_cmdline(const char *cmdline, hhg_str_t *stdouterr);
#endif

// turns a Hedgehog source filename into its corresponding executable filename
const char *hhg_file_to_exec(hhg_arena_t *arena, const char *name);

/*
supported format specifiers:

%s   - const char *
%i   - int
%i32 - int32_t
%i64 - int64_t
%u32 - uint32_t
%u64 - uint64_t
%f   - double
%li  - long
%lu  - unsigned long
%zu  - size_t
%c   - char
%b   - bool

%t   - hhg_token_type_t
%n   - hhg_node_type_t
%o   - hhg_mir_opnd_type_t
%r   - hhg_mir_reg_t
%l   - hhg_mir_lbl_t
%e   - hhg_mir_field_t

%S   - hhg_str_t *
%Y   - hhg_sym_t *
%N   - hhg_node_t * without symbols
%M   - hhg_node_t * with symbols
%T   - hhg_token_t *
%C   - hhg_type_t *
%O   - hhg_mir_opnd_t *
%I   - hhg_mir_instr_t *
%F   - hhg_mir_func_t *
%R   - hhg_file_range_t *
%P   - hhg_file_pos_t *
%%   - %
*/

void hhg_printf(const char *fmt, ...);

void hhg_fprintf(FILE *file, const char *fmt, ...);
void hhg_sprintf(hhg_str_t *str, const char *fmt, ...);

void hhg_vfprintf(FILE *file, const char *fmt, va_list va);
void hhg_vsprintf(hhg_str_t *str, const char *fmt, va_list va);

void hhg_stream_printf(const hhg_stream_t *stream, const char *fmt, ...);
void hhg_stream_vprintf(
    const hhg_stream_t *stream,
    const char *fmt,
    va_list args
);

const hhg_stream_t *hhg_stream_get_stdout();
const hhg_stream_t *hhg_stream_get_stderr();

// internal function for hhg_assert, not meant to be called directly
void hhg_assert_core(const char *expr_str, const char *file, int line);

#endif
