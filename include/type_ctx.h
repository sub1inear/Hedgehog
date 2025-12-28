#ifndef HHG_TYPE_CTX_H
#define HHG_TYPE_CTX_H

#include <stdbool.h>

#include <stb_ds.h>

#include "type.h"

typedef struct hhg_ref_tab_key {
    hhg_type_t *base;
    bool is_const;
    bool is_volatile;
} hhg_ref_tab_key_t;

typedef struct hhg_arr_tab_key {
    hhg_type_t *elem;
    size_t size;
} hhg_arr_tab_key_t;

typedef struct hhg_cv_tab_key {
    hhg_type_t *base;
    bool is_const;
    bool is_volatile;
} hhg_cv_tab_key_t;

typedef struct hhg_ref_tab {
    hhg_ref_tab_key_t key; // base type
    hhg_type_t value; // reference type
} hhg_ref_tab_t;

typedef struct hhg_arr_tab {
    hhg_arr_tab_key_t key;
    hhg_type_t value; // array type
} hhg_arr_tab_t;

typedef struct hhg_cv_tab {
    hhg_cv_tab_key_t key;
    hhg_type_t value;
} hhg_cv_tab_t;

typedef struct hhg_type_ctx {
    hhg_arena_t *arena;

    hhg_type_t builtins[HHG_BUILTIN_TYPE_COUNT];

    hhg_arr_tab_t *arr_tab;
    hhg_ref_tab_t *ref_tab;
    hhg_cv_tab_t *cv_tab;
} hhg_type_ctx_t;


void hhg_type_ctx_init(hhg_type_ctx_t *type_ctx, hhg_arena_t *arena);

hhg_type_t *hhg_type_ctx_get_builtin(
    hhg_type_ctx_t *type_ctx,
    hhg_base_type_t base
);

hhg_type_t *hhg_type_ctx_new_ref(
    hhg_type_ctx_t *type_ctx,
    hhg_ref_tab_key_t key
);

hhg_type_t *hhg_type_ctx_new_arr(
    hhg_type_ctx_t *ctx,
    hhg_arr_tab_key_t key
);

hhg_type_t *hhg_type_ctx_new_cv_type(
    hhg_type_ctx_t *type_ctx,
    hhg_cv_tab_key_t key
);

void hhg_type_ctx_del(hhg_type_ctx_t *ctx);

#endif
