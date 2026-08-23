#include <stdio.h>

#include "mem.h"
#include "sym.h"
#include "type.h"

hhg_sym_t *hhg_sym_new(hhg_arena_t *arena, const char *key,
                       hhg_sym_value_t value)
{
    hhg_sym_t *sym = hhg_arena_malloc(arena, sizeof(hhg_sym_t));
    *sym = (hhg_sym_t){
        .key = key,
        .value = value,
    };
    return sym;
}

void hhg_sym_print(hhg_sym_t *sym)
{
    hhg_sym_fprint(sym, stdout);
}

void hhg_sym_fprint(hhg_sym_t *sym, FILE *stream)
{
    fprintf(stream, "%s: ", sym->key);
    hhg_type_fprint(sym->value.type, stream);
}

void hhg_sym_deinit(hhg_sym_t *sym)
{
    // string is in arena/static memory, so no need to free
    // sym is in arena (allocated by sym tab), so no need to free
    if (sym->value.type)
        hhg_type_deinit(sym->value.type);
}
