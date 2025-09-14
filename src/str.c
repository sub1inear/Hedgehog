#include <string.h>
#include <assert.h>

#include "str.h"
#include "mem.h"

void hhg_str_init(hhg_str_t *str)
{
    str->len = 1;
    str->cap = 8;
    
    str->str = hhg_malloc(str->cap * sizeof(*str->str));
    str->str[0] = '\0';
}

void hhg_str_init_len(hhg_str_t *str, size_t len)
{
    assert(len > 0);

    str->len = len + 1;
    str->cap = str->len;

    str->str = hhg_malloc(str->cap * sizeof(*str->str));
    str->str[0] = '\0';
}

void hhg_str_init_str(hhg_str_t *str, const char *init, size_t len)
{
    assert(len > 0);
    assert(init != NULL);
    assert(len == strlen(init) + 1);

    str->len = len + 1;
    str->cap = str->len;

    str->str = hhg_malloc(str->cap * sizeof(*str->str));
    strcpy(str->str, init);
}

void hhg_str_init_copy(hhg_str_t *dest, hhg_str_t *src)
{
    dest->len = src->len;
    dest->cap = src->cap;
    dest->str = hhg_malloc(dest->cap * sizeof(*dest->str));
    strcpy(dest->str, src->str);
}

void hhg_str_reset(hhg_str_t *str)
{
    str->len = 1;
    str->str[0] = '\0';
}

void hhg_str_copy(hhg_str_t *dest, hhg_str_t *src)
{
    dest->len = src->len;
    if (dest->cap < src->cap)
        hhg_str_set_cap(dest, src->cap);
    strcpy(dest->str, src->str);
}

void hhg_str_append_char(hhg_str_t *str, char c)
{
    str->len++;
    if (str->len > str->cap)
        hhg_str_set_cap(str, str->len * 2);

    str->str[str->len - 2] = c;
    str->str[str->len - 1] = '\0';

}

void hhg_str_append_str(hhg_str_t *str, const char *append, size_t len)
{
    assert(len > 0);
    assert(append != NULL);
    assert(len == strlen(append) + 1);

    char *start = str->str + str->len + 1;

    str->len += len;
    if (str->len > str->cap)
        hhg_str_set_cap(str, str->len * 2);

    strcpy(start, append);
}

void hhg_str_set_cap(hhg_str_t *str, size_t cap)
{
    assert(cap > 0);
    str->cap = cap;
    str->str = hhg_realloc(str->str, str->cap);
}

void hhg_str_del(hhg_str_t *str)
{
    hhg_free(str->str);
}