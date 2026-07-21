#include <stb_ds.h>

#include "mem.h"
#include "type.h"
#include "type_ctx.h"
#include "utils.h"

void hhg_type_ctx_init(hhg_type_ctx_t *type_ctx, hhg_arena_t *arena)
{
    for (hhg_base_type_t i = HHG_BUILTIN_TYPE_START; i < HHG_BUILTIN_TYPE_END;
         i++)
        hhg_type_init(&type_ctx->builtins[i - HHG_BUILTIN_TYPE_START], i);
    type_ctx->tab = NULL;

    type_ctx->arena = arena;
}

hhg_type_t *hhg_type_ctx_get_builtin(hhg_type_ctx_t *type_ctx,
                                     hhg_base_type_t base)
{
    hhg_assert(base >= HHG_BUILTIN_TYPE_START && base < HHG_BUILTIN_TYPE_END);
    return &type_ctx->builtins[base - HHG_BUILTIN_TYPE_START];
}

hhg_type_t *hhg_type_ctx_new_type(hhg_type_ctx_t *type_ctx, hhg_type_t type)
{
    hhg_type_ctx_tab_t *entry = hmgetp_null(type_ctx->tab, type);

    if (entry != NULL)
        return entry->value;

    hhg_type_t *new_type =
        hhg_arena_malloc(type_ctx->arena, sizeof(hhg_type_t));
    *new_type = type;
    hmput(type_ctx->tab, type, new_type);
    return new_type;
}

void hhg_type_ctx_del(hhg_type_ctx_t *type_ctx)
{
    hmfree(type_ctx->tab);
}
