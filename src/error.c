#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <inttypes.h>
#include <assert.h>

#include <stb_ds.h>

#include "error.h"
#include "mem.h"
#include "file_pos.h"
#include "str.h"
#include "token.h"

typedef struct _hhg_msg_t {
    hhg_msg_type_t type;
    hhg_file_pos_t pos;
    const char *filename;
    char *str;
} hhg_msg_t;

static hhg_msg_t *msgs = NULL;

static char *hhg_vasprintf(const char *fmt, va_list va);
static void hhg_vfprintf(FILE *stream, const char *fmt, va_list va);

static void hhg_msg_print(hhg_msg_t *msg)
{
    switch (msg->type) {
    case HHG_MSG_ERROR:
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
            msg->filename, msg->pos.line, msg->pos.col);

    fputs(msg->str, stderr);
    
    fputs("\n" "\x1b[0m", stderr);
}

static void hhg_msg_del(hhg_msg_t *msg)
{
    hhg_free_s(msg->str);
}

void hhg_msg(hhg_msg_type_t type,
             hhg_file_pos_t pos,
             const char *filename,
             const char *fmt,
             ...)
{
    assert(filename != NULL);

    va_list va;

    va_start(va, fmt);

    char *str = hhg_vasprintf(fmt, va);

    va_end(va);

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
    hhg_vfprintf(stderr, fmt, va);
    fputs("\n" "\x1b[0m", stderr);

    va_end(va);
    exit(1);
}

bool hhg_msgs_has_errors(void)
{
    size_t len = arrlenu(msgs);
    for (size_t i = 0; i < len; i++)
        if (msgs[i].type == HHG_MSG_ERROR)
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

static void hhg_printf_common(
    const char *fmt,
    va_list va,
    void *in,
    void (*out_char)(void *, int),
    void (*out_str)(void *, const char *)
)
{
    char c;
    while ((c = *fmt++)) {
        if (c == '%')
            switch (c = *fmt++) {
            case 's': {
                char *str_arg = va_arg(va, char *);
                if (str_arg)
                    out_str(in, str_arg);
                else
                    out_str(in, "(null)"); 
                break;
            }
            case 'd': {
                int32_t int_arg = va_arg(va, int32_t);
                uint32_t uint_arg;

                if (int_arg < 0) {
                    out_char(in, '-');                    
                    uint_arg = (uint32_t)(-(int64_t)int_arg);
                } else
                    uint_arg = (uint32_t)int_arg;
                
                uint32_t div = 1;
                while (uint_arg / div >= 10)
                    div *= 10;

                while (div) {
                    out_char(in, '0' + uint_arg / div);
                    uint_arg %= div;
                    div /= 10;
                }
                break;
            }
            case 'c': {
                char char_arg = va_arg(va, char);
                out_char(in, char_arg);
                break;
            }
            case 'b': {
                bool bool_arg = va_arg(va, bool);
                if (bool_arg)
                    out_str(in, "true");
                else
                    out_str(in, "false");
                break;
            }
            case '%':
                out_char(in, c);
                break;
            case 't': {
                hhg_token_type_t token_type_arg =
                    va_arg(va, hhg_token_type_t);
                const char *token_type_str =
                     hhg_token_type_to_str(token_type_arg);
                out_str(in, token_type_str);
                break;
            }
            default:
                break;
            }
        else
            out_char(in, c);
    }
}

// avoid warnings with void *
static void hhg_printf_append_char(void *str, int c)
{
    hhg_str_append_char(str, c);
}

static void hhg_printf_append_str(void *str, const char *append)
{
    hhg_str_append_str(str, append);
}

static char *hhg_vasprintf(const char *fmt, va_list va)
{
    hhg_str_t str;
    hhg_str_init(&str);

    hhg_printf_common(
        fmt,
        va,
        &str,
        hhg_printf_append_char,
        hhg_printf_append_str
    );

    return str.str;
}

// avoid warnings with void * and swap argument order
static void hhg_printf_fputc(void *stream, int c)
{
    fputc(c, stream);
}

static void hhg_printf_fputs(void *stream, const char *str)
{
    fputs(str, stream);
}

static void hhg_vfprintf(FILE *stream, const char *fmt, va_list va)
{
    hhg_printf_common(
        fmt,
        va,
        stream,
        hhg_printf_fputc,
        hhg_printf_fputs
    );
}