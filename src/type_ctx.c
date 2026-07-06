#include "type_ctx.h"

#include <stb_ds.h>

#include "utils.h"

void hhg_type_ctx_init(hhg_type_ctx_t *type_ctx, hhg_arena_t *arena)
{
    for (hhg_base_type_t i = HHG_BUILTIN_TYPE_START; i < HHG_BUILTIN_TYPE_END;
         i++)
        hhg_type_init(&type_ctx->builtins[i - HHG_BUILTIN_TYPE_START], i);

    type_ctx->ref_tab = NULL;
    type_ctx->arr_tab = NULL;

    type_ctx->arena = arena;
}

hhg_type_t *hhg_type_ctx_get_builtin(hhg_type_ctx_t *type_ctx,
                                     hhg_base_type_t base)
{
    if (base < HHG_BUILTIN_TYPE_START || base >= HHG_BUILTIN_TYPE_END) {
        return NULL;
    }
    return &type_ctx->builtins[base - HHG_BUILTIN_TYPE_START];
}

hhg_type_t *hhg_type_ctx_new_ref(hhg_type_ctx_t *type_ctx,
                                 hhg_ref_tab_key_t key)
{
    hhg_ref_tab_t *entry = hmgetp_null(type_ctx->ref_tab, key);

    if (entry != NULL)
        return &entry->value;

    hhg_type_t ref = {
        .info.ref.base = key.base,
        .is_const = key.is_const,
        .is_volatile = key.is_volatile,
    };

    hmput(type_ctx->ref_tab, key, ref);

    hhg_ref_tab_t *new_entry = hmgetp_null(type_ctx->ref_tab, key);
    hhg_assert(new_entry != NULL);
    return &new_entry->value;
}

hhg_type_t *hhg_type_ctx_new_arr(hhg_type_ctx_t *type_ctx,
                                 hhg_arr_tab_key_t key)
{
    hhg_arr_tab_t *entry = hmgetp_null(type_ctx->arr_tab, key);
    if (entry != NULL)
        return &entry->value;

    hhg_type_t arr = {
        .info.arr = {
                     .elem = key.elem,
                     .size = key.size,
                     }
    };

    hmput(type_ctx->arr_tab, key, arr);

    hhg_arr_tab_t *new_entry = hmgetp_null(type_ctx->arr_tab, key);
    hhg_assert(new_entry != NULL);
    return &new_entry->value;
}

hhg_type_t *hhg_type_ctx_new_cv_type(hhg_type_ctx_t *type_ctx,
                                     hhg_cv_tab_key_t key)
{
    hhg_cv_tab_t *entry = hmgetp_null(type_ctx->cv_tab, key);
    if (entry != NULL)
        return &entry->value;

    hhg_type_t cv_type = *key.base;
    cv_type.is_const = key.is_const;
    cv_type.is_volatile = key.is_volatile;

    hmput(type_ctx->cv_tab, key, cv_type);

    hhg_cv_tab_t *new_entry = hmgetp_null(type_ctx->cv_tab, key);
    hhg_assert(new_entry != NULL);
    return &new_entry->value;
}

void hhg_type_ctx_del(hhg_type_ctx_t *type_ctx)
{
    hmfree(type_ctx->arr_tab);
    hmfree(type_ctx->ref_tab);
}
