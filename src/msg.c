#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>

#include "msg.h"
#include "file_pos.h"
#include "file_src.h"
#include "file_range.h"
#include "token.h"
#include "node.h"
#include "type.h"
#include "cmd_args.h"
#include "main.h"
#include "utils.h"

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

// prints the message type and increments error count if it's an error
static void hhg_msg_process_msg_type(
    hhg_msg_ctx_t *msg_ctx,
    hhg_msg_type_t type
);

// prints the message type (error, warning, ...) with color if enabled
// if color is not enabled, color can be NULL
static void hhg_msg_print_msg_type_str(const char *str, const char *color);

static void hhg_vfprintf(FILE *stream, const char *fmt, va_list args);

void hhg_msg_ctx_init(hhg_msg_ctx_t *msg_ctx, hhg_cmd_args_t *cmd_args)
{
    msg_ctx->error_count = 0;
    msg_ctx->cmd_args = cmd_args;
}

void hhg_msg_ctx_del(hhg_msg_ctx_t *msg_ctx)
{
    HHG_UNUSED(msg_ctx);
    // nothing to free for now
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

    hhg_msg_process_msg_type(msg_ctx, type);

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
        vfprintf(stderr, note, va_note);
    }

    fputs("\n\n", stderr);

    va_end(va_note);
    va_end(va_msg);
}

void hhg_basic_msg(
    hhg_msg_ctx_t *msg_ctx,
    hhg_msg_type_t type,
    const char *msg,
    ...
)
{
    va_list va;
    va_start(va, msg);

    hhg_msg_process_msg_type(msg_ctx, type);
    hhg_vfprintf(stderr, msg, va);

    fputc('\n', stderr);
    va_end(va);
}

void hhg_compiler_error(const char *msg, ...)
{
    va_list va;
    va_start(va, msg);
    
    fputs("compiler error: ", stderr);

    hhg_vfprintf(stderr, msg, va);

    fprintf(
        stderr,
        "\n\nplease report this to the developers at\n"
        HHG_GITHUB_ISSUES_URL "\n"
    );

    va_end(va);
    exit(EXIT_FAILURE);
}

void hhg_fatal_error(const char *msg, ...)
{
    va_list va;
    va_start(va, msg);
    
    fputs("fatal error: ", stderr);

    vfprintf(stderr, msg, va);

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

static void hhg_msg_process_msg_type(
    hhg_msg_ctx_t *msg_ctx,
    hhg_msg_type_t type
)
{
    if (msg_ctx->cmd_args->type == HHG_CMD_ARGS_BUILD &&
        msg_ctx->cmd_args->subcmd.build.error_warnings &&
        type == HHG_MSG_WARNING)
        type = HHG_MSG_ERROR;
    switch (type) {
    case HHG_MSG_ERROR:
        msg_ctx->error_count++;
        hhg_msg_print_msg_type_str("error", HHG_ANSI_COLOR_RED);
        break;
    case HHG_MSG_WARNING:
        if (msg_ctx->cmd_args->type == HHG_CMD_ARGS_BUILD &&
            !msg_ctx->cmd_args->subcmd.build.warnings)
            break;
        hhg_msg_print_msg_type_str("warning", HHG_ANSI_COLOR_YELLOW);
        break;
    case HHG_MSG_INFO:
        hhg_msg_print_msg_type_str("info", NULL);
        break;
    }
}

static void hhg_msg_print_msg_type_str(const char *str, const char *color)
{
    fprintf(stderr, "%s%s" HHG_ANSI_COLOR_CLEAR ": ", color ? color : "", str);
}

static void hhg_vfprintf(FILE *stream, const char *fmt, va_list va)
{
    char c;
    while ((c = *fmt++) != '\0') {
        if (c == '%') {
            switch (c = *fmt++) {
            case 's': {
                const char *str_arg = va_arg(va, const char *);
                fputs(str_arg ? str_arg : "(null)", stream);
                break;
            }
            case 'i':
                fprintf(stream, "%i", va_arg(va, int));
                break;
            case 'l': {
                c = *++fmt;
                if (c == 'u')
                    fprintf(stream, "%lu", va_arg(va, unsigned long));
                else
                    hhg_compiler_error("unknown format specifier: %%%c", c);
                break;
            }
            case 'c':
                fputc((char)va_arg(va, int), stream);
                break;
            case 'b':
                fputs((bool)va_arg(va, int) ? "true" : "false", stream);
                break;
            // lexer-only: %n and %T disabled (node.c and type.c are #if 0'd)
            case 'n':
                HHG_UNUSED(va_arg(va, hhg_node_type_t));
                break;
            case 't':
                hhg_token_type_fprint(va_arg(va, hhg_token_type_t), stream);
                break;
            case 'T':
                HHG_UNUSED(va_arg(va, hhg_type_t *));
                break;
            case '%':
                fputc('%', stream);
                break;
            default:
                hhg_compiler_error("unknown format specifier: %%%c", c);
                break;
            }
            
        } else
            fputc(c, stream);
    }
}