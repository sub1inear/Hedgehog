#ifndef HHG_SYM_H
#define HHG_SYM_H

#include "type.h"

typedef struct hhg_sym {
    char *key;
    hhg_type_t value;
} hhg_sym_t;

void hhg_sym_print(hhg_sym_t *sym);

#endif
