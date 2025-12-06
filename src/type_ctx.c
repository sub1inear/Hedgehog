#include "type_ctx.h"

void hhg_type_ctx_init(hhg_type_ctx_t *type_ctx, hhg_arena_t *arena)
{
    for (hhg_base_type_t i = HHG_BUILTIN_TYPE_START;
         i < HHG_BUILTIN_TYPE_END;
         i++)
        hhg_type_init(&type_ctx->builtins[i - HHG_BUILTIN_TYPE_START], i);

    type_ctx->ref_tab = NULL;
    type_ctx->arr_tab = NULL;

    type_ctx->arena = arena;
}

hhg_type_t *hhg_type_ctx_get_builtin(
    hhg_type_ctx_t *type_ctx,
    hhg_base_type_t base
)
{
    if (base < HHG_BUILTIN_TYPE_START ||
        base >= HHG_BUILTIN_TYPE_END) {
        return NULL;
    }
    return &type_ctx->builtins[base - HHG_BUILTIN_TYPE_START];
}

hhg_type_t *hhg_type_ctx_new_ref(
    hhg_type_ctx_t *type_ctx,
    hhg_ref_tab_key_t key
)
{
    hhg_ref_tab_t *entry = hmgetp_null(type_ctx->ref_tab, key);

    if (entry != NULL)
        return entry->value;

    hhg_type_t *ref = hhg_type_new(HHG_TYPE_REF, type_ctx->arena);

    ref->info.ref.base = key.base;
    ref->is_const = key.is_const;
    ref->is_volatile = key.is_volatile;

    hmput(type_ctx->ref_tab, key, ref);
    
    return ref;
}

hhg_type_t *hhg_type_ctx_new_arr(
    hhg_type_ctx_t *type_ctx,
    hhg_arr_tab_key_t key
)
{
    hhg_arr_tab_t *entry = hmgetp_null(type_ctx->arr_tab, key);
    if (entry != NULL)
        return entry->value;
    hhg_type_t *arr = hhg_type_new(HHG_TYPE_ARR, type_ctx->arena);

    arr->info.arr.elem = key.elem;
    arr->info.arr.size = key.size;

    hmput(type_ctx->arr_tab, key, arr);
    return arr;
}

void hhg_type_ctx_del(hhg_type_ctx_t *type_ctx)
{
    hmfree(type_ctx->arr_tab);
    hmfree(type_ctx->ref_tab);
}