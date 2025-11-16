#include <stdint.h>
#include <assert.h>

#include "sym_tab.h"
#include "sym.h"
#include "mem.h"

#define STB_DS_IMPLEMENTATION
#define STBDS_REALLOC(context, ptr, size) hhg_realloc(ptr, size)
#define STBDS_FREE(context, ptr) hhg_free(ptr)
#include <stb_ds.h>

void hhg_sym_tab_init(hhg_sym_tab_t *sym_tab)
{
    sym_tab->tab = NULL;
    sym_tab->key_arr = NULL;
    hhg_sym_tab_enter_scope(sym_tab);
}

void hhg_sym_tab_enter_scope(hhg_sym_tab_t *sym_tab)
{
    arrput(sym_tab->key_arr, NULL);
}

hhg_sym_t *hhg_sym_tab_insert(hhg_sym_tab_t *sym_tab, hhg_sym_t sym)
{
    // inserts pointer to sym into sym_tab
    shputs(sym_tab->tab, sym);

    size_t len = arrlenu(sym_tab->key_arr);
    assert(len > 0);

    // pushes sym str onto last entry of sym_key_arr
    size_t last = len - 1;
    arrput(sym_tab->key_arr[last], sym.key);

    hhg_sym_t *p = shgetp_null(sym_tab->tab, sym.key);
    assert(p != NULL);
    return p;
}

hhg_sym_t *hhg_sym_tab_lookup(hhg_sym_tab_t *sym_tab, const char *key)
{
    return shgetp_null(sym_tab->tab, key);
}


void hhg_sym_tab_exit_scope(hhg_sym_tab_t *sym_tab)
{
    // remove all strs from last entry of sym_key_arr
    size_t outer_len = arrlenu(sym_tab->key_arr);
    if (outer_len > 0) {
        size_t last = outer_len - 1;
        size_t inner_len = arrlenu(sym_tab->key_arr[last]);
        for (size_t i = 0; i < inner_len; i++)
            (void)shdel(sym_tab->tab, sym_tab->key_arr[last][i]);
        arrfree(sym_tab->key_arr[last]);
        (void)arrpop(sym_tab->key_arr);
    }
}

void hhg_sym_tab_clear(hhg_sym_tab_t *sym_tab)
{
    hhg_sym_tab_del(sym_tab);
}

void hhg_sym_tab_del(hhg_sym_tab_t *sym_tab)
{
    hhg_sym_tab_exit_scope(sym_tab);
    shfree(sym_tab->tab);
    assert(arrlenu(sym_tab->key_arr) == 0);
    arrfree(sym_tab->key_arr);
}
