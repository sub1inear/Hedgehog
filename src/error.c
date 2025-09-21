#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <inttypes.h>
#include <assert.h>

#include <stb_ds.h>

#include "error.h"
#include "mem.h"
#include "file_pos.h"

typedef struct _hhg_msg_t {
    hhg_msg_type_t type;
    hhg_file_pos_t pos;
    const char *filename;
    char *str;
} hhg_msg_t;

static hhg_msg_t *msgs = NULL;

static void hhg_msg_print(hhg_msg_t *msg)
{
    switch (msg->type) {
    case ERROR:
        fputs("\x1b[1;31m" "error: ", stderr);
        break;
    case WARNING:
        fputs("\x1b[1;33m" "warning: ", stderr);
        break;
    case INFO:
        fputs("info: ", stderr);
        break;
    }

    fprintf(stderr, "%s:%" PRIi32  ":%" PRIi32 ": ",
            msg->filename, msg->pos.line, msg->pos.col);

    fputs(msg->str, stderr);
    
    fputs("\n" "\x1b[0m", stderr);
}

static void hhg_msg_del(hhg_msg_t *msg)
{
    hhg_free(msg->str);
}

void hhg_msg(hhg_msg_type_t type,
             hhg_file_pos_t pos,
             const char *filename,
             const char *fmt,
             ...)
{
    assert(filename != NULL);

    va_list va;
    va_list va_c;

    va_start(va, fmt);
    va_copy(va_c, va);

    int size = vsnprintf(NULL, 0, fmt, va);
    
    va_end(va);
    
    char *str = hhg_malloc(size + 1);

    vsprintf(str, fmt, va_c);
    
    va_end(va_c);

    // need temp var as compound literals do not work in macros
    hhg_msg_t msg = { type, pos, filename, str};
    arrput(msgs, msg);
}

void hhg_fatal_error(const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    
    hhg_msgs_print();
    hhg_msgs_del();

    fputs("\x1b[1;31m" "fatal error: ", stderr);
    vfprintf(stderr, fmt, va);
    fputs("\n" "\x1b[0m", stderr);

    va_end(va);
    exit(1);
}

bool hhg_msgs_has_errors(void)
{
    size_t len = arrlenu(msgs);
    for (size_t i = 0; i < len; i++)
        if (msgs[i].type == ERROR)
            return true;
    return false;
}

void hhg_msgs_print(void)
{
    size_t len = arrlenu(msgs);
    for (size_t i = 0; i < len; i++)
        hhg_msg_print(&msgs[i]);
}

void hhg_msgs_del(void)
{
    size_t len = arrlenu(msgs);
    for (size_t i = 0; i < len; i++)
        hhg_msg_del(&msgs[i]);

    arrfree(msgs);
}
