#include <stdio.h>
#include <stdarg.h>

#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>

#include "error.h"
#include "mem.h"

typedef struct _hhg_msg_t {
    char *str;
    hhg_msg_type_t type;
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

    fputs(msg->str, stderr);
    
    fputs("\n" "\x1b[0m", stderr);
}

static void hhg_msg_del(hhg_msg_t *msg)
{
    hhg_free(msg->str);
}

void hhg_msg(hhg_msg_type_t type, const char *fmt,  ...)
{
    va_list va;
    va_list va_c;

    va_start(va, fmt);
    va_copy(va_c, va);

    int size = vsnprintf(NULL, 0, fmt, va);
    
    va_end(va);
    
    char *buf = hhg_malloc(size + 1);

    vsprintf(buf, fmt, va_c);
    
    va_end(va_c);

    // need temp var as compound literals do not work in macros
    hhg_msg_t msg = { buf, type };
    arrput(msgs, msg);
}

void hhg_fatal_error(const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    
    hhg_msgs_print();
    hhg_msgs_del();

    fputs("fatal error: ", stderr);
    vfprintf(stderr, fmt, va);
    putchar('\n');

    va_end(va);
    exit(1);
}

void hhg_msgs_print()
{
    size_t len = arrlenu(msgs);
    for (size_t i = 0; i < len; i++)
        hhg_msg_print(&msgs[i]);
}

void hhg_msgs_del()
{
    size_t len = arrlenu(msgs);
    for (size_t i = 0; i < len; i++)
        hhg_msg_del(&msgs[i]);

    arrfree(msgs);
}