#include "mem.h"

#define STB_DS_IMPLEMENTATION
#define STBDS_REALLOC(context, ptr, size) hhg_realloc(ptr, size)
#define STBDS_FREE(context, ptr) hhg_free(ptr)
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
    return shgetp_null(sym_tab, key);
}

void hhg_sym_del(void)
{
    shfree(sym_tab);
}
