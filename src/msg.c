#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <inttypes.h>
#include <assert.h>

#include "msg.h"
#include "lexer.h"
#include "mem.h"
#include "file_pos.h"
#include "file_src.h"
#include "file_range.h"
#include "str.h"
#include "token.h"
#include "type.h"

#define HHG_ANSI_COLOR_CLEAR "\x1b[0m"
#define HHG_ANSI_COLOR_RED "\x1b[1;31m"
#define HHG_ANSI_COLOR_YELLOW "\x1b[1;33m"

/*
Message format:
<level>: <message>
       --> <filename>:<line>:<col>
 <line> | <source line>
        | ^~~~ <note> (optional)
(repeat for each line in range)
*/

static int32_t hhg_msg_num_digits(int32_t num);
static void hhg_msg_print_src_line(
    hhg_file_src_t *src,
    int32_t line,
    int32_t line_width
);
static void hhg_msg_print_indicator(
    int32_t start,
    int32_t end,
    size_t line_width
);

/*
simplified vfprintf with support for hhg-specific types

supported format specifiers:
%s - string
%d - int32_t
%c - char
%b - bool
%% - %
%t - hhg_token_type_t
%n - hhg_node_type_t 
%T - hhg_type_t *
*/
static void hhg_vfprintf(FILE *stream, const char *fmt, va_list va);

void hhg_msg_ctx_init(hhg_msg_ctx_t *msg_ctx)
{
    msg_ctx->error_count = 0;
}

void hhg_msg(
    hhg_msg_ctx_t *msg_ctx,
    hhg_msg_type_t type,
    hhg_file_src_t *src,
    hhg_file_range_t *range,
    const char *msg,
    const char *note,
    ...
)
{
    va_list va_msg;
    va_list va_note;
    va_start(va_msg, note);
    va_copy(va_note, va_msg);

    switch (type) {
    case HHG_MSG_ERROR:
        msg_ctx->error_count++;
        fputs(HHG_ANSI_COLOR_RED "error" HHG_ANSI_COLOR_CLEAR ": ", stderr);
        break;
    case HHG_MSG_WARNING:
        fputs(
            HHG_ANSI_COLOR_YELLOW "warning" HHG_ANSI_COLOR_CLEAR ": ",
            stderr
        );
        break;
    case HHG_MSG_INFO:
        fputs("info: ", stderr);
        break;
    }

    hhg_vfprintf(stderr, msg, va_msg);

    int32_t max_line_width = 0;
    for (int32_t line = range->start.line; line <= range->end.line; line++) {
        // line is 0-indexed but printed as 1-indexed so need to increment
        int32_t line_width = hhg_msg_num_digits(line + 1);
        if (line_width > max_line_width)
            max_line_width = line_width;
    }

    fputc('\n', stderr);

    for (int32_t i = 0; i < max_line_width; i++)
        fputc(' ', stderr);

    fprintf(stderr, "--> %s:", src->filename);

    hhg_file_pos_fprint(&range->start, stderr);

    fputc('\n', stderr);

    for (int32_t line = range->start.line; line <= range->end.line; line++) {
        hhg_msg_print_src_line(src, line, max_line_width);

        int32_t start_col = line == range->start.line ? range->start.col : 0;
        int32_t end_col = line == range->end.line ? range->end.col :
            src->line_starts[line + 1] - src->line_starts[line];

        hhg_msg_print_indicator(start_col, end_col, max_line_width);
        if (line != range->end.line)
            fputc('\n', stderr);
    }

    if (note) {
        fputc(' ', stderr);
        hhg_vfprintf(stderr, note, va_note);
    }

    fputs("\n\n", stderr);

    va_end(va_note);
    va_end(va_msg);
}

void hhg_fatal_error(const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);

    fputs(HHG_ANSI_COLOR_RED "fatal error" HHG_ANSI_COLOR_CLEAR ": ", stderr);
    hhg_vfprintf(stderr, fmt, va);
    fputc('\n', stderr);
    va_end(va);
    exit(EXIT_FAILURE);
}

// could be faster but simplicity is more important
static int32_t hhg_msg_num_digits(int32_t num)
{
    if (num <= 0)
        return 0;

    if (num >= 1000000000) return 10;
    if (num >= 100000000)  return 9;
    if (num >= 10000000)   return 8;
    if (num >= 1000000)    return 7;
    if (num >= 100000)     return 6;
    if (num >= 10000)      return 5;
    if (num >= 1000)       return 4;
    if (num >= 100)        return 3;
    if (num >= 10)         return 2;
    return 1;
}

static void hhg_msg_print_src_line(
    hhg_file_src_t *src,
    int32_t line,
    int32_t line_width
)
{
    fprintf(stderr, "%*" PRIi32 " | ", line_width, line + 1);
    
    char *ptr = &src->txt[src->line_starts[line]];

    // src->txt is null-terminated
    while (*ptr != '\n' && *ptr != '\0') {
        fputc(*ptr, stderr);
        ptr++;
    }
    fputc('\n', stderr);
}

static void hhg_msg_print_indicator(
    int32_t start,
    int32_t end,
    size_t line_width
)
{
    for (size_t i = 0; i < line_width; i++)
        fputc(' ', stderr);
    fputs(" | ", stderr);
    for (int32_t i = 0; i < start; i++)
        fputc(' ', stderr);
    fputc('^', stderr);
    for (int32_t i = start + 1; i < end; i++)
        fputc('~', stderr);
}

static void hhg_vfprintf(FILE *stream, const char *fmt, va_list va)
{
    char c;
    // (()) to avoid warning about assignment in condition
    while ((c = *fmt++)) {
        if (c == '%') {
            switch (c = *fmt) {
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
                fputc('%', stream);
                break;
            case 'n': // same as 't'
            case 't': {
                hhg_token_type_t token_type_arg =
                    va_arg(va, hhg_token_type_t);
                const char *token_type_str =
                     hhg_token_type_to_str(token_type_arg);
                fputs(token_type_str, stream);
                break;
            }
            case 'T': {
                hhg_type_t *type_arg = va_arg(va, hhg_type_t *);
                hhg_type_fprint(type_arg, stream);
                break;
            }
            default:
                break;
            }
            if (c != '\0')
                fmt++;
        } else
            fputc(c, stream);
    }
}