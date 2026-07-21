#ifndef HHG_SYM_H
#define HHG_SYM_H

#include "type.h"

typedef enum hhg_sym_type {
    HHG_SYM_VAR,

    HHG_SYM_FUNC,
    HHG_SYM_CLASS,
    HHG_SYM_ENUM,

    HHG_SYM_ENUM_CONST,

    HHG_SYM_PARAM,
    HHG_SYM_FIELD,
} hhg_sym_type_t;

typedef struct hhg_sym_value_t {
    hhg_sym_type_t sym_type;
    hhg_type_t *type;
} hhg_sym_value_t;

typedef struct hhg_sym {
    const char *key;
    hhg_sym_value_t value;
} hhg_sym_t;

// mainly allocated in hhg_sym_tab_insert but temporary symbols too
hhg_sym_t *hhg_sym_new(hhg_arena_t *arena, const char *key,
                       hhg_sym_value_t value);

void hhg_sym_print(hhg_sym_t *sym);
void hhg_sym_fprint(hhg_sym_t *sym, FILE *stream);

void hhg_sym_del(hhg_sym_t *sym);

#endif
