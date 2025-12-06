#include <stdio.h>

#include "sym.h"
#include "type.h"

void hhg_sym_print(hhg_sym_t *sym)
{
    hhg_type_print(sym->value.type);
    printf(" %s\n", sym->key);
}

void hhg_sym_del(hhg_sym_t *sym)
{
    // string is in parser arena/static memory, so no need to free
    // sym may not have a type (for classes declared in parser)
    if (sym->value.type != NULL)
        hhg_type_del(sym->value.type);
}
