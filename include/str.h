#ifndef HHG_STR_H
#define HHG_STR_H

typedef struct hhg_str {
    size_t len, cap;
    char *str;
} hhg_str_t;

void hhg_str_init(hhg_str_t *str);
void hhg_str_init_len(hhg_str_t *str, size_t len);
void hhg_str_init_str(hhg_str_t *str, const char *init);
void hhg_str_init_copy(hhg_str_t *dst, hhg_str_t *src);
void hhg_str_init_fmt(hhg_str_t *str, const char *fmt, ...);

hhg_str_t *hhg_str_new(void);
hhg_str_t *hhg_str_new_len(size_t len);
hhg_str_t *hhg_str_new_str(const char *init);
hhg_str_t *hhg_str_new_copy(hhg_str_t *src);
hhg_str_t *hhg_str_new_fmt(const char *fmt, ...);

void hhg_str_reset(hhg_str_t *str);

void hhg_str_copy(hhg_str_t *dst, hhg_str_t *src);

void hhg_str_append_char(hhg_str_t *str, int c);
void hhg_str_append_str(hhg_str_t *str, const char *append);
void hhg_str_append_hhg_str(hhg_str_t *dst, hhg_str_t *append);
void hhg_str_append_str_len(hhg_str_t *str, const char *append, size_t len);
void hhg_str_append_fmt(hhg_str_t *str, const char *fmt, ...);

char hhg_str_pop(hhg_str_t *str);

void hhg_str_set_cap(hhg_str_t *str, size_t cap);

void hhg_str_del(hhg_str_t *str);
void hhg_str_free(hhg_str_t *str);

#endif
