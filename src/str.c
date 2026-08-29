#include <stdarg.h>
#include <string.h>

#include "mem.h"
#include "str.h"
#include "utils.h"

void hhg_str_init(hhg_str_t *str)
{
    str->len = 0;
    str->cap = 8;

    str->str = hhg_malloc(str->cap * sizeof(*str->str));
    str->str[0] = '\0';
}

void hhg_str_init_len(hhg_str_t *str, size_t len)
{
    str->len = 0;
    str->cap = len + 1;

    str->str = hhg_malloc(str->cap * sizeof(*str->str));
    str->str[0] = '\0';
}

void hhg_str_init_str(hhg_str_t *str, const char *init)
{
    hhg_assert(init != NULL);

    str->len = strlen(init);
    str->cap = str->len + 1;

    str->str = hhg_malloc(str->cap * sizeof(*str->str));
    strcpy(str->str, init);
}

void hhg_str_init_copy(hhg_str_t *dst, hhg_str_t *src)
{
    dst->len = src->len;
    dst->cap = src->cap;
    dst->str = hhg_malloc(dst->cap * sizeof(*dst->str));
    strcpy(dst->str, src->str);
}

void hhg_str_init_fmt(hhg_str_t *str, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    hhg_str_init_vfmt(str, fmt, va);
    va_end(va);
}

void hhg_str_init_vfmt(hhg_str_t *str, const char *fmt, va_list va)
{
    va_list va_fmt;
    va_copy(va_fmt, va);

    int len = vsnprintf(NULL, 0, fmt, va);
    va_end(va);

    hhg_str_init_len(str, (size_t)len);

    vsnprintf(str->str, str->cap, fmt, va_fmt);
    va_end(va_fmt);
}

hhg_str_t *hhg_str_new(void)
{
    hhg_str_t *str = hhg_malloc(sizeof(*str));
    hhg_str_init(str);
    return str;
}

hhg_str_t *hhg_str_new_len(size_t len)
{
    hhg_str_t *str = hhg_malloc(sizeof(*str));
    hhg_str_init_len(str, len);
    return str;
}

hhg_str_t *hhg_str_new_str(const char *init)
{
    hhg_str_t *str = hhg_malloc(sizeof(*str));
    hhg_str_init_str(str, init);
    return str;
}

hhg_str_t *hhg_str_new_copy(hhg_str_t *src)
{
    hhg_str_t *str = hhg_malloc(sizeof(*str));
    hhg_str_init_copy(str, src);
    return str;
}

hhg_str_t *hhg_str_new_fmt(const char *fmt, ...)
{
    hhg_str_t *str = hhg_malloc(sizeof(*str));
    va_list va;
    va_start(va, fmt);
    hhg_str_init_vfmt(str, fmt, va);
    va_end(va);
    return str;
}

hhg_str_t *hhg_str_new_vfmt(const char *fmt, va_list va)
{
    hhg_str_t *str = hhg_malloc(sizeof(*str));
    hhg_str_init_vfmt(str, fmt, va);
    return str;
}

void hhg_str_reset(hhg_str_t *str)
{
    str->len = 0;
    str->str[0] = '\0';
}

void hhg_str_copy(hhg_str_t *dst, hhg_str_t *src)
{
    dst->len = src->len;
    if (dst->cap <= src->len)
        hhg_str_set_cap(dst, src->cap);
    strcpy(dst->str, src->str);
}

void hhg_str_append_char(hhg_str_t *str, int c)
{
    str->len++;
    if (str->len >= str->cap)
        hhg_str_set_cap(str, str->len * 2);

    str->str[str->len - 1] = (char)c;
    str->str[str->len] = '\0';
}

void hhg_str_append_str(hhg_str_t *str, const char *append)
{
    hhg_str_append_str_len(str, append, strlen(append));
}

void hhg_str_append_hhg_str(hhg_str_t *dst, hhg_str_t *append)
{
    hhg_str_append_str_len(dst, append->str, append->len);
}

void hhg_str_append_str_len(hhg_str_t *str, const char *append, size_t len)
{
    size_t prev_len = str->len;

    str->len += len;

    if (str->len >= str->cap)
        hhg_str_set_cap(str, str->len * 2);

    memcpy(str->str + prev_len, append, len);
    str->str[str->len] = '\0';
}

void hhg_str_append_fmt(hhg_str_t *str, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int len = vsnprintf(NULL, 0, fmt, args);
    size_t prev_len = str->len;

    str->len += (size_t)len;
    if (str->len >= str->cap)
        hhg_str_set_cap(str, str->len * 2);

    vsnprintf(str->str + prev_len, str->cap - prev_len, fmt, args);

    va_end(args);
}

char hhg_str_pop(hhg_str_t *str)
{
    char c = str->str[str->len - 1];
    str->str[--str->len] = '\0';
    return c;
}

void hhg_str_set_cap(hhg_str_t *str, size_t cap)
{
    hhg_assert(cap > 0);
    str->cap = cap;
    if (str->len >= str->cap) {
        str->len = str->cap - 1;
        str->str[str->len] = '\0';
    }

    str->str = hhg_realloc(str->str, str->cap * sizeof(*str->str));
}

void hhg_str_deinit(hhg_str_t *str)
{
    hhg_free_s(str->str);
}

void hhg_str_free(hhg_str_t *str)
{
    hhg_str_deinit(str);
    hhg_free(str);
}
