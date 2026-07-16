#include <stdio.h>

#include "sym.h"
#include "mem.h"
#include "type.h"
#include "str.h"

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
    hhg_sym_fprint(sym, stdout);
}

void hhg_sym_fprint(hhg_sym_t *sym, FILE *stream)
{
    // lexer-only: hhg_type_fprint disabled (type.c is #if 0'd)
    fprintf(stream, " %s\n", sym->key);
}

void hhg_sym_del(hhg_sym_t *sym)
{
    // string is in arena/static memory, so no need to free
    // sym is in arena (allocated by sym tab), so no need to free
    // lexer-only: hhg_type_del disabled (type.c is #if 0'd)
    (void)sym;
}
