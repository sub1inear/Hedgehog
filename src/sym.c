#include <stdio.h>

#include "sym.h"
#include "type.h"

hhg_sym_t *hhg_sym_new(
    hhg_arena_t *arena,
    const char *key,
    hhg_sym_value_t value
)
{
    hhg_sym_t *sym = hhg_arena_malloc(arena, sizeof(hhg_sym_t));
    *sym = (hhg_sym_t) {
        .key = key,
        .value = value,
    };
    return sym;
}

void hhg_sym_print(hhg_sym_t *sym)
{
    hhg_type_print(sym->value.type);
    printf(" %s\n", sym->key);
}

void hhg_sym_del(hhg_sym_t *sym)
{
    // string is in arena/static memory, so no need to free
    // sym is in arena (allocated by sym tab), so no need to free
    // sym may not have a type (for classes declared in parser)
    if (sym->value.type != NULL)
        hhg_type_del(sym->value.type);
}
