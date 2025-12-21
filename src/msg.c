#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <inttypes.h>
#include <assert.h>

#include "msg.h"
#include "mem.h"
#include "file_pos.h"
#include "str.h"
#include "token.h"

static int32_t error_count;

void hhg_msg(
    hhg_msg_type_t type,
    hhg_file_pos_t pos,
    const char *filename,
    const char *fmt,
    ...
)
{
    va_list va;

    va_start(va, fmt);

    switch (type) {
    case HHG_MSG_ERROR:
        error_count++;
        fputs("\x1b[1;31m" "error: ", stderr);
        break;
    case HHG_MSG_WARNING:
        fputs("\x1b[1;33m" "warning: ", stderr);
        break;
    case HHG_MSG_INFO:
        fputs("info: ", stderr);
        break;
    }

    fprintf(stderr, "%s:%" PRIi32  ":%" PRIi32 ": ",
            filename, pos.line, pos.col);

    vfprintf(stderr, fmt, va);
    
    fputs("\n" "\x1b[0m", stderr);

    va_end(va);
}

void hhg_fatal_error(const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);

    fputs("\x1b[1;31m" "fatal error: ", stderr);
    vfprintf(stderr, fmt, va);
    fputs("\n" "\x1b[0m", stderr);

    va_end(va);
    exit(EXIT_FAILURE);
}

int32_t hhg_msgs_get_error_count(void)
{
    return error_count;
}
