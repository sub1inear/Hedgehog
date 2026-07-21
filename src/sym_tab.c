#include <stb_ds.h>

#include "mem.h"
#include "sym.h"
#include "sym_tab.h"
#include "utils.h"

void hhg_sym_tab_init(hhg_sym_tab_t *sym_tab, hhg_arena_t *arena)
{
    *sym_tab = (hhg_sym_tab_t){
        .tab = NULL,
        .key_arr = NULL,
        .arena = arena,
    };
}

void hhg_sym_tab_enter_scope(hhg_sym_tab_t *sym_tab)
{
    arrput(sym_tab->key_arr, NULL);
}

hhg_sym_t *hhg_sym_tab_insert(hhg_sym_tab_t *sym_tab, hhg_sym_t sym)
{
    hhg_sym_t *new_sym = hhg_arena_malloc(sym_tab->arena, sizeof(hhg_sym_t));
    *new_sym = sym;

    // inserts pointer to sym into sym_tab
    pshput(sym_tab->tab, new_sym);

    size_t len = arrlenu(sym_tab->key_arr);
    hhg_assert(len > 0);

    // pushes sym str onto last entry of sym_key_arr
    size_t last = len - 1;
    arrput(sym_tab->key_arr[last], new_sym->key);

    return new_sym;
}

hhg_sym_t *hhg_sym_tab_lookup(hhg_sym_tab_t *sym_tab, const char *key)
{
    return pshget_null(sym_tab->tab, key);
}

void hhg_sym_tab_exit_scope(hhg_sym_tab_t *sym_tab)
{
    // sym_tab->key_arr stores keys in each scope
    // when exiting a scope, all entries in sym_tab->key_arr[last]
    // are deleted from sym_tab->tab,
    // then sym_tab->key_arr[last] is freed and popped

    size_t outer_len = arrlenu(sym_tab->key_arr);
    if (outer_len > 0) {
        size_t last = outer_len - 1;
        size_t inner_len = arrlenu(sym_tab->key_arr[last]);
        for (size_t i = 0; i < inner_len; i++) {
            ptrdiff_t result = pshdel(sym_tab->tab, sym_tab->key_arr[last][i]);
            // ensure key was in table (otherwise corruption)
            hhg_assert(result);
        }
        arrfree(sym_tab->key_arr[last]);
        HHG_UNUSED(arrpop(sym_tab->key_arr));
    }
}

void hhg_sym_tab_clear(hhg_sym_tab_t *sym_tab)
{
    hhg_sym_tab_del(sym_tab);
}

void hhg_sym_tab_del(hhg_sym_tab_t *sym_tab)
{
    shfree(sym_tab->tab);
    hhg_assert(arrlenu(sym_tab->key_arr) == 0);
    arrfree(sym_tab->key_arr);
}
