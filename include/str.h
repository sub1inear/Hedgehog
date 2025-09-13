#ifndef HHG_STR_H
#define HHG_STR_H

#include <stdlib.h>

typedef struct _hhg_str_t {
    size_t len;
    size_t cap;
    char *str;
} hhg_str_t;

void hhg_str_init();
void hhg_str_init_len(hhg_str_t *str, size_t len);
void hhg_str_init_str(hhg_str_t *str, const char *init, size_t len);

void hhg_str_reset(hhg_str_t *str);

void hhg_str_append_char(hhg_str_t *str, char c);
void hhg_str_append_str(hhg_str_t *str, const char *append, size_t len);

void hhg_str_set_cap(hhg_str_t *str, size_t cap);

void hhg_str_del(hhg_str_t *str);

#endif
