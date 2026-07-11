#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <fs.h>

#include "utils.h"
#include "str.h"
#include "mem.h"
#include "msg.h"
#include "token.h"
#include "type.h"
#include "node.h"
#include "mir.h"
#include "file_pos.h"
#include "file_range.h"

#ifdef HHG_WINDOWS
#include <windows.h>
#elif defined(HHG_POSIX)
#include <unistd.h>
#include <sys/wait.h>
#endif

#define HHG_SPAWN_PIPE_FD_BUFFER_SIZE 4096

// easier to write HANDLE/fd-specific functions
// instead of converting into FILE * and then unifying them
#ifdef HHG_WINDOWS
static bool hhg_arg_needs_escape(const char *arg);
static hhg_str_t *hhg_escape_arg(const char *arg);
static void hhg_read_pipe_to_str(HANDLE pipe, hhg_str_t *out);
static int hhg_spawn_core(char *cmdline, const char *exec, hhg_str_t *stdouterr);
#elif defined(HHG_POSIX)
static void hhg_read_fd_to_str(int fd, hhg_str_t *out);
#endif
static void hhg_stream_file_out_str(void *arg, const char *str);
static void hhg_stream_file_out_char(void *arg, char c);
static void hhg_stream_str_out_str(void *arg, const char *str);
static void hhg_stream_str_out_char(void *arg, char c);
static void hhg_stream_print_int(const hhg_stream_t *stream, intmax_t num);
static void hhg_stream_print_uint(const hhg_stream_t *stream, uintmax_t num);
static void hhg_stream_print_double(const hhg_stream_t *stream, double num);

static hhg_stream_t stdout_stream = {
    .out_str = hhg_stream_file_out_str,
    .out_char = hhg_stream_file_out_char,
};

static hhg_stream_t stderr_stream = {
    .out_str = hhg_stream_file_out_str,
    .out_char = hhg_stream_file_out_char,
};

FILE *hhg_fopen(const char *filename, const char *mode)
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

void hhg_join_path(
    char *buf,
    size_t size,
    const char *left,
    const char *right
)
{
    int result = fs_join_path(buf, size, left, right);
    if (result >= size)
        hhg_fatal_error(
            "joined path is too long: `%s` `%s`",
            left,
            right
        );
    else if (result < 0)
        hhg_fatal_error(
            "failed to join paths: `%s` `%s`",
            left,
            right
        );
}

#ifdef HHG_WINDOWS
int hhg_spawn(const char **argv, hhg_str_t *stdouterr)
{
    hhg_str_t cmd;
    hhg_str_init(&cmd);

    for (size_t i = 0; argv[i] != NULL; i++) {
        if (i > 0)
            hhg_str_append_char(&cmd, ' ');
        if (hhg_arg_needs_escape(argv[i])) {
            hhg_str_t *arg = hhg_escape_arg(argv[i]);
            hhg_str_append_hhg_str(&cmd, arg);
            hhg_str_free(arg);
        } else
            hhg_str_append_str(&cmd, argv[i]);
    }

    int exit_code = hhg_spawn_core(cmd.str, argv[0], stdouterr);
    hhg_str_del(&cmd);
    return exit_code;
}

int hhg_spawn_cmdline(const char *cmdline, hhg_str_t *stdouterr)
{
    char *cmdline_mut = hhg_strdup(cmdline);
    int exit_code = hhg_spawn_core(cmdline_mut, NULL, stdouterr);
    hhg_free(cmdline_mut);
    return exit_code;
}

#elif defined(HHG_POSIX)
int hhg_spawn(const char **argv, hhg_str_t *stdouterr)
{
    int pipefd[2] = { -1, -1 };

    if (stdouterr != NULL)
        if (pipe(pipefd) != 0)
            hhg_fatal_error("pipe failed: %s", strerror(errno));

    pid_t pid = fork();
    if (pid < 0)
        hhg_fatal_error("fork failed: %s", strerror(errno));

    if (pid == 0) {
        if (stdouterr != NULL) {
            dup2(pipefd[1], STDOUT_FILENO);
            dup2(pipefd[1], STDERR_FILENO);
            close(pipefd[0]);
            close(pipefd[1]);
        }
        execvp(argv[0], (char *const *)argv);
        _exit(127);
    }

    if (stdouterr != NULL) {
        close(pipefd[1]);
        hhg_read_fd_to_str(pipefd[0], stdouterr);
        close(pipefd[0]);
    }

    int status = 0;
    while (waitpid(pid, &status, 0) < 0) {
        if (errno == EINTR)
            continue;
        hhg_fatal_error("waitpid failed: %s", strerror(errno));
    }

    if (WIFEXITED(status))
        return WEXITSTATUS(status);
    if (WIFSIGNALED(status))
        return 128 + WTERMSIG(status);

    return 1;
}
#endif

#ifdef HHG_WINDOWS
const char *hhg_file_to_exec(hhg_arena_t *arena, const char *name)
{
    size_t name_len = strlen(name);
    size_t ext_len = HHG_STR_LEN(".exe");
    size_t new_ext_start = name_len - ext_len;
    char *exec = hhg_arena_malloc(arena, name_len + ext_len + 1);
    memcpy(exec, name, new_ext_start);
    strcpy(exec + new_ext_start, ".exe");
    return exec;
}
#elif HHG_POSIX
const char *hhg_file_to_exec(hhg_arena_t *arena, const char *name)
{
    HHG_UNUSED(arena);
    return name;
}
#endif

int64_t hhg_str_to_int64(const char *str)
{
    const char *ptr = str;
    bool negative = false;
    if (*ptr == '-') {
        negative = true;
        ptr++;
    }
    int64_t result = 0;
    while (*ptr) {
        if (*ptr < '0' || *ptr > '9')
            hhg_fatal_error("invalid integer: %s", str);
        if (result > (INT64_MAX - (*ptr - '0')) / 10)
            hhg_fatal_error("integer overflow: %s", str);
        result = result * 10 + (*ptr - '0');
        ptr++;
    }
    return negative ? -result : result;
}

void hhg_printf(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    hhg_stream_vprintf(hhg_stream_get_stdout(), fmt, va);
    va_end(va);
}

void hhg_fprintf(FILE *stream, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    hhg_vfprintf(stream, fmt, va);
    va_end(va);
}

void hhg_sprintf(hhg_str_t *str, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    hhg_vsprintf(str, fmt, va);
    va_end(va);
}

void hhg_vfprintf(FILE *file, const char *fmt, va_list va)
{
    hhg_stream_vprintf(
        &(hhg_stream_t) {
            .out_str = hhg_stream_file_out_str,
            .out_char = hhg_stream_file_out_char,
            .arg = file
        },
        fmt,
        va
    );
}

void hhg_vsprintf(hhg_str_t *str, const char *fmt, va_list va)
{
    hhg_stream_vprintf(
        &(hhg_stream_t) {
            .out_str = hhg_stream_str_out_str,
            .out_char = hhg_stream_str_out_char,
            .arg = str
        },
        fmt,
        va
    );
}

void hhg_stream_printf(const hhg_stream_t *stream, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    hhg_stream_vprintf(stream, fmt, va);
    va_end(va);
}

void hhg_stream_vprintf(
    const hhg_stream_t *stream,
    const char *fmt,
    va_list va
)
{
    char c;
    while ((c = *fmt++) != '\0') {
        if (c == '%') {
            switch (c = *fmt) {
            case 's': {
                const char *str_arg = va_arg(va, const char *);
                stream->out_str(stream->arg, str_arg ? str_arg : "(null)");
                break;
            }
            case 'i':
                c = *++fmt;
                switch (c) {
                case '3':
                    c = *++fmt;
                    if (c == '2') {
                        hhg_stream_print_int(stream, va_arg(va, int32_t));
                        break;
                    } else
                        hhg_compiler_error(
                            "invalid format specifier: `%%%c%c`",
                            '3',
                            c
                        );
                case '6':
                    c = *++fmt;
                    if (c == '4') {
                        hhg_stream_print_int(stream, va_arg(va, int64_t));
                        break;
                    } else
                        hhg_compiler_error(
                            "invalid format specifier: `%%%c%c`",
                            '6',
                            c
                        );
                default:
                    hhg_stream_print_int(stream, va_arg(va, int));
                    break;
                }
                break;
            case 'u':
                c = *++fmt;
                switch (c) {
                case '3':
                    c = *++fmt;
                    if (c == '2') {
                        hhg_stream_print_uint(stream, va_arg(va, uint32_t));
                        break;
                    } else
                        hhg_compiler_error(
                            "invalid format specifier: `%%%c%c`",
                            '3',
                            c
                        );
                case '6':
                    c = *++fmt;
                    if (c == '4') {
                        hhg_stream_print_uint(stream, va_arg(va, uint64_t));
                        break;
                    } else
                        hhg_compiler_error(
                            "invalid format specifier: `%%%c%c`",
                            '6',
                            c
                        );
                default:
                    hhg_stream_print_uint(stream, va_arg(va, unsigned int));
                    break;
                }
                break;
            case 'f':
                hhg_stream_print_double(stream, va_arg(va, double));
                break;
            case 'l':
                c = *++fmt;
                switch (c) {
                case 'i':
                    hhg_stream_print_int(stream, va_arg(va, long));
                    break;
                case 'u':
                    hhg_stream_print_uint(stream, va_arg(va, unsigned long));
                    break;
                default:
                    hhg_stream_print_int(stream, va_arg(va, hhg_mir_lbl_t));
                    break;
                }
                break;
            case 'z': {
                fmt++;
                c = *fmt;
                if (c == 'u')
                    hhg_stream_print_uint(stream, va_arg(va, size_t));
                else
                    hhg_compiler_error("invalid format specifier: `%%%c`", c);
                break;
            }
            case 'c':
                stream->out_char(stream->arg, (char)va_arg(va, int));
                break;
            case 'b':
                stream->out_str(
                    stream->arg,
                    (bool)va_arg(va, int) ? "true" : "false"
                );
                break;
            case 'n': // same as 't'
            case 't':
                hhg_token_type_print_stream(
                    va_arg(va, hhg_token_type_t),
                    stream
                );
                break;
            case 'r':
                hhg_stream_print_int(stream, va_arg(va, hhg_mir_reg_t));
                break;
            case 'e':
                hhg_stream_print_int(stream, va_arg(va, hhg_mir_field_t));
                break;
            case 'S': {
                hhg_str_t *hhg_str_arg = va_arg(va, hhg_str_t *);
                if (hhg_str_arg) {
                    hhg_assert(hhg_str_arg->str != NULL);
                    stream->out_str(stream->arg, hhg_str_arg->str);
                } else
                    stream->out_str(stream->arg, "(null)");
                break;
            }
            case 'Y': {
                hhg_sym_t *sym_arg = va_arg(va, hhg_sym_t *);
                if (sym_arg)
                    hhg_sym_print_stream(sym_arg, stream);
                else
                    stream->out_str(stream->arg, "(null)");
                break;
            }
            case 'M':
            case 'N': {
                hhg_node_t *node_arg = va_arg(va, hhg_node_t *);
                if (node_arg)
                    hhg_node_print_stream(
                        node_arg,
                        HHG_NODE_INDENT_START,
                        c == 'M' ? true : false,
                        stream
                    );
                else
                    stream->out_str(stream->arg, "(null)");
                break;
            }
            case 'T':
                hhg_token_print_stream(va_arg(va, hhg_token_t *), stream);
                break;
            case 'C':
                hhg_type_print_stream(va_arg(va, hhg_type_t *), stream);
                break;
            case 'R': {
                hhg_file_range_t *file_range_arg =
                    va_arg(va, hhg_file_range_t *);
                if (file_range_arg)
                    hhg_file_range_print_stream(file_range_arg, stream);
                else
                    stream->out_str(stream->arg, "(null)");
                break;
            }
            case 'P': {
                hhg_file_pos_t *file_pos_arg =
                    va_arg(va, hhg_file_pos_t *);
                if (file_pos_arg)
                    hhg_file_pos_print_stream(file_pos_arg, stream);
                else
                    stream->out_str(stream->arg, "(null)");
                break;
            }
            case '%':
                stream->out_str(stream->arg, "%%");
                break;
            default:
                hhg_compiler_error("invalid format specifier: `%%%c`", c);
                break;
            }
            if (c != '\0')
                fmt++;
        } else
            stream->out_char(stream->arg, c);
    }
}

const hhg_stream_t *hhg_stream_get_stdout()
{   
    stdout_stream.arg = stdout; // stdout can be a function so must be reassigned
    return (const hhg_stream_t *)&stdout_stream;
}

const hhg_stream_t *hhg_stream_get_stderr()
{
    stderr_stream.arg = stderr; // stderr can be a function so must be reassigned
    return (const hhg_stream_t *)&stderr_stream;
}



void hhg_assert_core(const char *expr_str, const char *file, int line)
{
    hhg_compiler_error(
        "assertion failed: %s, at %s:%i",
        expr_str,
        file,
        line
    );
}

#ifdef HHG_WINDOWS
static bool hhg_arg_needs_escape(const char *arg)
{
    for (const char *ptr = arg; *ptr != '\0'; ptr++) {
        if (*ptr == ' ' || *ptr == '\t' || *ptr == '\n')
            return true;
    }
    return false;
}

static hhg_str_t *hhg_escape_arg(const char *arg)
{
    hhg_str_t *out = hhg_str_new_str("\"");

    int backslashes = 0;
    for (const char *ptr = arg; *ptr != '\0'; ptr++) {
        if (*ptr == '\\') {
            backslashes++;
            continue;
        }

        if (*ptr == '"') {
            for (int i = 0; i < backslashes * 2 + 1; i++)
                hhg_str_append_char(out, '\\');
            hhg_str_append_char(out, '"');
            backslashes = 0;
            continue;
        }

        for (int i = 0; i < backslashes; i++)
            hhg_str_append_char(out, '\\');
        backslashes = 0;
        hhg_str_append_char(out, *ptr);
    }

    for (int i = 0; i < backslashes * 2; i++)
        hhg_str_append_char(out, '\\');

    hhg_str_append_char(out, '"');
    return out;
}

static void hhg_read_pipe_to_str(HANDLE pipe, hhg_str_t *out)
{
    char buffer[HHG_SPAWN_PIPE_FD_BUFFER_SIZE];
    while (true) {
        DWORD len = 0;
        BOOL result = ReadFile(pipe, buffer, sizeof(buffer), &len, NULL);
        if (!result) {
            if (GetLastError() == ERROR_BROKEN_PIPE)
                break;
            hhg_fatal_error("ReadFile failed: %lu", GetLastError());
        }
        if (len == 0)
            break;
        hhg_str_append_str_len(out, buffer, (size_t)len);
    }
}

static int hhg_spawn_core(char *cmdline, const char *exec, hhg_str_t *stdouterr)
{
    STARTUPINFOA si = {
        .cb = sizeof(si),
    };
    PROCESS_INFORMATION pi = { 0 };

    HANDLE read_pipe = NULL;
    HANDLE write_pipe = NULL;

    if (stdouterr == NULL) {
        si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    } else {
        SECURITY_ATTRIBUTES sa = {
            .nLength = sizeof(sa),
            .lpSecurityDescriptor = NULL,
            .bInheritHandle = TRUE
        };

        if (!CreatePipe(&read_pipe, &write_pipe, &sa, 0))
            hhg_fatal_error("CreatePipe failed: %lu", GetLastError());

        if (!SetHandleInformation(read_pipe, HANDLE_FLAG_INHERIT, 0))
            hhg_fatal_error("SetHandleInformation failed: %lu", GetLastError());

        si.hStdOutput = write_pipe;
        si.hStdError = write_pipe;
    }

    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.dwFlags |= STARTF_USESTDHANDLES;

    BOOL result = CreateProcessA(
        NULL,
        cmdline,
        NULL,
        NULL,
        TRUE,
        0,
        NULL,
        NULL,
        &si,
        &pi
    );

    if (!result) {
        DWORD error = GetLastError();
        if (error == ERROR_FILE_NOT_FOUND && exec != NULL)
            hhg_fatal_error("executable not found: %s", exec);
        else
            hhg_fatal_error("CreateProcess failed: %lu", error);
    }

    if (write_pipe != NULL)
        CloseHandle(write_pipe);

    if (stdouterr != NULL)
        hhg_read_pipe_to_str(read_pipe, stdouterr);

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exit_code = 0;
    if (!GetExitCodeProcess(pi.hProcess, &exit_code))
        hhg_fatal_error("GetExitCodeProcess failed: %lu", GetLastError());

    if (read_pipe != NULL)
        CloseHandle(read_pipe);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return (int)exit_code;
}

#elif defined(HHG_POSIX)
static void hhg_read_fd_to_str(int fd, hhg_str_t *out)
{
    char buffer[HHG_SPAWN_PIPE_FD_BUFFER_SIZE];
    ssize_t len;
    while ((len = read(fd, buffer, sizeof(buffer))) > 0)
        hhg_str_append_str_len(out, buffer, (size_t)len);
    if (len < 0)
        hhg_fatal_error("error reading file descriptor: %s", strerror(errno));
}
#endif

static void hhg_stream_file_out_str(void *arg, const char *str)
{
    fputs(str, (FILE *)arg);
}

static void hhg_stream_file_out_char(void *arg, char c)
{
    fputc(c, (FILE *)arg);
}

static void hhg_stream_str_out_str(void *arg, const char *str)
{
    hhg_str_append_str((hhg_str_t *)arg, str);
}

static void hhg_stream_str_out_char(void *arg, char c)
{
    hhg_str_append_char((hhg_str_t *)arg, c);
}

static void hhg_stream_print_int(const hhg_stream_t *stream, intmax_t num)
{
    uintmax_t unum;
    if (num < 0) {
        stream->out_char(stream->arg, '-');
        unum = (uintmax_t)(-num);
    } else {
        unum = (uintmax_t)num;
    }
    hhg_stream_print_uint(stream, unum);
}

static void hhg_stream_print_uint(const hhg_stream_t *stream, uintmax_t num)
{
    char buffer[32];
    size_t i = 0;

    do {
        buffer[i++] = (char)(num % 10) + '0';
        num /= 10;
    } while (num > 0);

    while (i > 0)
        stream->out_char(stream->arg, buffer[--i]);
}

static void hhg_stream_print_double(const hhg_stream_t *stream, double num)
{
    if (isnan(num)) {
        stream->out_str(stream->arg, "nan");
        return;
    }
    if (isinf(num)) {
        stream->out_str(stream->arg, num < 0.0 ? "-inf" : "inf");
        return;
    }

    if (num < 0.0) {
        stream->out_char(stream->arg, '-');
        num = -num;
    }

    bool use_sci = num != 0.0 && (num >= 1e7 || num < 1e-4);
    int exp = 0;
    if (use_sci) {
        if (num >= 10.0) {
            while (num >= 10.0) { num /= 10.0; exp++; }
        } else {
            while (num < 1.0) { num *= 10.0; exp--; }
        }
    }

    intmax_t int_part = (intmax_t)num;
    double frac = num - (double)int_part;
    uint64_t frac_digits = (uint64_t)(frac * 1000000.0 + 0.5);
    if (frac_digits >= 1000000) {
        frac_digits = 0;
        int_part++;
        if (use_sci && int_part >= 10) {
            int_part = 1;
            exp++;
        }
    }

    hhg_stream_print_int(stream, int_part);
    stream->out_char(stream->arg, '.');

    char frac_buf[7];
    for (int i = 5; i >= 0; i--) {
        frac_buf[i] = '0' + (int)(frac_digits % 10);
        frac_digits /= 10;
    }
    frac_buf[6] = '\0';
    stream->out_str(stream->arg, frac_buf);

    if (use_sci) {
        stream->out_char(stream->arg, 'e');
        stream->out_char(stream->arg, exp >= 0 ? '+' : '-');
        hhg_stream_print_uint(stream, (uintmax_t)(exp >= 0 ? exp : -exp));
    }
}