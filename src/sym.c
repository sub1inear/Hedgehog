#include <stb_ds.h>

#include "sym.h"

static hhg_sym_t* sym_tab;

void hhg_sym_init()
{
    sh_new_arena(sym_tab);
}

void hhg_sym_insert(hhg_sym_t *sym)
{
    shput(sym_tab, sym->key, sym->value);
}

hhg_sym_t *hhg_sym_lookup(const char *key)
{
    return shgetp(sym_tab, key);
}

void hhg_sym_del(void)
{
    shfree(sym_tab);
}
