#ifndef HHG_TYPE_CTX_H
#define HHG_TYPE_CTX_H

#include <stdbool.h>

#include "type.h"

typedef struct hhg_type_ctx_tab {
    hhg_type_t key;
    hhg_type_t *value;
} hhg_type_ctx_tab_t;

typedef struct hhg_type_ctx {
    hhg_arena_t *arena;
    hhg_type_t builtins[HHG_BUILTIN_TYPE_COUNT];
    hhg_type_ctx_tab_t *tab;
} hhg_type_ctx_t;

void hhg_type_ctx_init(hhg_type_ctx_t *type_ctx, hhg_arena_t *arena);

hhg_type_t *hhg_type_ctx_get_builtin(hhg_type_ctx_t *type_ctx,
                                     hhg_base_type_t base);

hhg_type_t *hhg_type_ctx_new_type(hhg_type_ctx_t *type_ctx, hhg_type_t type);

void hhg_type_ctx_del(hhg_type_ctx_t *ctx);

#endif
