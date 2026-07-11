#include <stdio.h>

#include "sym.h"
#include "mem.h"
#include "type.h"
#include "utils.h"

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
    hhg_sym_print_stream(sym, hhg_stream_get_stdout());
}

void hhg_sym_print_stream(hhg_sym_t *sym, const hhg_stream_t *stream)
{
    hhg_type_print_stream(sym->value.type, stream);
    hhg_stream_printf(stream, " %s\n", sym->key);
}

void hhg_sym_del(hhg_sym_t *sym)
{
    // string is in arena/static memory, so no need to free
    // sym is in arena (allocated by sym tab), so no need to free
    // sym may not have a type (for classes declared in parser)
    if (sym->value.type != NULL)
        hhg_type_del(sym->value.type);
}
