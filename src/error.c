#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <inttypes.h>
#include <assert.h>

#include "error.h"
#include "mem.h"
#include "file_pos.h"
#include "str.h"
#include "token.h"

static int32_t error_count;

static void hhg_vfprintf(FILE *stream, const char *fmt, va_list va);

void hhg_msg(hhg_msg_type_t type,
             hhg_file_pos_t pos,
             const char *filename,
             const char *fmt,
             ...)
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

    hhg_vfprintf(stderr, fmt, va);
    
    fputs("\n" "\x1b[0m", stderr);

    va_end(va);
}

void hhg_fatal_error(const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);

    fputs("\x1b[1;31m" "fatal error: ", stderr);
    hhg_vfprintf(stderr, fmt, va);
    fputs("\n" "\x1b[0m", stderr);

    va_end(va);
    exit(EXIT_FAILURE);
}

int32_t hhg_msgs_get_error_count()
{
    return error_count;
}

static void hhg_vfprintf(FILE *stream, const char *fmt, va_list va)
{
    char c;
    while ((c = *fmt++)) {
        if (c == '%')
            switch (c = *fmt++) {
            case 's': {
                const char *str_arg = va_arg(va, const char *);
                if (str_arg)
                    fputs(str_arg, stream);
                else
                    fputs("(null)", stream); 
                break;
            }
            case 'd': {
                int32_t int_arg = va_arg(va, int32_t);
                uint32_t uint_arg;

                if (int_arg < 0) {
                    fputc('-', stream);                    
                    uint_arg = (uint32_t)(-(int64_t)int_arg);
                } else
                    uint_arg = (uint32_t)int_arg;
                
                uint32_t div = 1;
                while (uint_arg / div >= 10)
                    div *= 10;

                while (div) {
                    fputc('0' + uint_arg / div, stream);
                    uint_arg %= div;
                    div /= 10;
                }
                break;
            }
            case 'c': {
                char char_arg = (char)va_arg(va, int);
                fputc(char_arg, stream);
                break;
            }
            case 'b': {
                bool bool_arg = (bool)va_arg(va, int);
                if (bool_arg)
                    fputs("true", stream);
                else
                    fputs("false", stream);
                break;
            }
            case '%':
                fputc(c, stream);
                break;
            case 't': {
                hhg_token_type_t token_type_arg =
                    va_arg(va, hhg_token_type_t);
                const char *token_type_str =
                     hhg_token_type_to_str(token_type_arg);
                fputs(token_type_str, stream);
                break;
            }
            default:
                break;
            }
        else
            fputc(c, stream);
    }
}