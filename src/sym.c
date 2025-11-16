#include <stdio.h>

#include "sym.h"

void hhg_sym_print(hhg_sym_t *sym)
{
    hhg_type_print(&sym->value);
    printf(" %s\n", sym->key);
}