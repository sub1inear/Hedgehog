#include "utils.h"

#include <fs.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "mem.h"
#include "msg.h"
#include "str.h"

#ifdef HHG_WINDOWS
#include <windows.h>
#elif defined(HHG_POSIX)
#include <sys/wait.h>
#include <unistd.h>
#endif

#ifdef HHG_POSIX
#define HHG_UTILS_PIPE_READER_BUF_SIZE 4096
#endif

// easier to write HANDLE/fd-specific functions
// instead of converting into FILE * and then unifying them
#ifdef HHG_WINDOWS
static bool hhg_utils_arg_needs_escape(const char *arg);
static hhg_str_t *hhg_utils_escape_arg(const char *arg);
static void hhg_utils_read_pipe_to_str(HANDLE pipe, hhg_str_t *out);
#elif defined(HHG_POSIX)
static void hhg_utils_read_fd_to_str(int fd, hhg_str_t *out);
#endif

FILE *hhg_utils_fopen(const char *filename, const char *mode)
{
    FILE *file = fopen(filename, mode);
    if (file == NULL)
        hhg_fatal_error("opening %s: %s", filename, strerror(errno));
    return file;
}

void hhg_utils_join_path(char *buf, size_t size, const char *left,
                         const char *right)
{
    int result = fs_join_path(buf, size, left, right);
    if (result >= size)
        hhg_fatal_error("joined path is too long: `%s` `%s`", left, right);
    else if (result < 0)
        hhg_fatal_error("failed to join paths: `%s` `%s`", left, right);
}

#ifdef HHG_WINDOWS
int hhg_utils_spawn(const char **argv, hhg_str_t *stdouterr)
{
    hhg_str_t cmd;
    hhg_str_init(&cmd);

    for (size_t i = 0; argv[i] != NULL; i++) {
        if (i > 0)
            hhg_str_append_char(&cmd, ' ');
        if (hhg_utils_arg_needs_escape(argv[i])) {
            hhg_str_t *arg = hhg_utils_escape_arg(argv[i]);
            hhg_str_append_hhg_str(&cmd, arg);
            hhg_str_free(arg);
        } else
            hhg_str_append_str(&cmd, argv[i]);
    }

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
        SECURITY_ATTRIBUTES sa = { .nLength = sizeof(sa),
                                   .lpSecurityDescriptor = NULL,
                                   .bInheritHandle = TRUE };

        if (!CreatePipe(&read_pipe, &write_pipe, &sa, 0))
            hhg_fatal_error("CreatePipe failed: %lu", GetLastError());

        if (!SetHandleInformation(read_pipe, HANDLE_FLAG_INHERIT, 0))
            hhg_fatal_error("SetHandleInformation failed: %lu", GetLastError());

        si.hStdOutput = write_pipe;
        si.hStdError = write_pipe;
    }

    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.dwFlags |= STARTF_USESTDHANDLES;

    BOOL result = CreateProcessA(NULL, cmd.str, NULL, NULL, TRUE, 0, NULL, NULL,
                                 &si, &pi);

    if (!result) {
        DWORD error = GetLastError();
        if (error == ERROR_FILE_NOT_FOUND)
            hhg_fatal_error("executable not found: %s", argv[0]);
        else
            hhg_fatal_error("CreateProcess failed: %lu", error);
    }

    if (write_pipe != NULL)
        CloseHandle(write_pipe);

    if (stdouterr != NULL)
        hhg_utils_read_pipe_to_str(read_pipe, stdouterr);

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exit_code = 0;
    if (!GetExitCodeProcess(pi.hProcess, &exit_code))
        hhg_fatal_error("GetExitCodeProcess failed: %lu", GetLastError());

    if (read_pipe != NULL)
        CloseHandle(read_pipe);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    hhg_str_del(&cmd);
    return (int)exit_code;
}

#elif defined(HHG_POSIX)
int hhg_utils_spawn(const char **argv, hhg_str_t *stdouterr)
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
        hhg_utils_read_fd_to_str(pipefd[0], stdouterr);
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
const char *hhg_utils_file_to_exec(hhg_arena_t *arena, const char *name)
{
    size_t name_len = strlen(name);
    size_t ext_len = sizeof(".exe") - 1;
    char *exec = hhg_arena_malloc(arena, name_len + ext_len + 1);
    strcpy(exec, name);
    strcpy(exec + name_len, ".exe");
    return exec;
}
#elif HHG_POSIX
const char *hhg_utils_file_to_exec(hhg_arena_t *arena, const char *name)
{
    HHG_UNUSED(arena);
    return name;
}
#endif

int64_t hhg_utils_str_to_int64(const char *str)
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

void hhg_utils_assert(const char *expr_str, const char *file, int line)
{
    hhg_compiler_error("assertion failed: %s, at %s:%i", expr_str, file, line);
}

#ifdef HHG_WINDOWS
static bool hhg_utils_arg_needs_escape(const char *arg)
{
    for (const char *ptr = arg; *ptr != '\0'; ptr++) {
        if (*ptr == ' ' || *ptr == '\t' || *ptr == '\n')
            return true;
    }
    return false;
}

static hhg_str_t *hhg_utils_escape_arg(const char *arg)
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

static void hhg_utils_read_pipe_to_str(HANDLE pipe, hhg_str_t *out)
{
    char buf[4096];
    while (true) {
        DWORD len = 0;
        BOOL result = ReadFile(pipe, buf, sizeof(buf) - 1, &len, NULL);
        if (!result) {
            if (GetLastError() == ERROR_BROKEN_PIPE)
                break;
            hhg_fatal_error("ReadFile failed: %lu", GetLastError());
        }
        if (len == 0)
            break;
        hhg_str_append_str_len(out, buf, (size_t)len);
    }
}

#elif defined(HHG_POSIX)
static void hhg_utils_read_fd_to_str(int fd, hhg_str_t *out)
{
    char buf[HHG_UTILS_PIPE_READER_BUF_SIZE + 1];
    ssize_t len;
    while ((len = read(fd, buf, HHG_UTILS_PIPE_READER_BUF_SIZE)) > 0)
        hhg_str_append_str_len(out, buf, (size_t)len);
    if (len < 0)
        hhg_fatal_error("read failed: %s", strerror(errno));
}
#endif
