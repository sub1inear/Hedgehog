#ifndef HHG_SYM_H
#define HHG_SYM_H

#include "type.h"
#include "str.h"

typedef struct _hhg_sym_t {
    const char *key;
    hhg_type_t value;
} hhg_sym_t;

void hhg_sym_init();

void hhg_sym_insert(hhg_sym_t *sym);
hhg_sym_t *hhg_sym_lookup(const char *key);

void hhg_sym_del(void);

#endif