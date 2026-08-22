#include <stb_ds.h>

#include "mem.h"
#include "sym.h"
#include "sym_tab.h"
#include "utils.h"

void hhg_sym_tab_init(hhg_sym_tab_t *sym_tab, hhg_arena_t *arena)
{
    *sym_tab = (hhg_sym_tab_t){
        .tab = NULL,
        .arena = arena,
    };
}

void hhg_sym_tab_enter_scope(hhg_sym_tab_t *sym_tab)
{
    arrput(sym_tab->tab, NULL);
}

hhg_sym_t *hhg_sym_tab_insert(hhg_sym_tab_t *sym_tab, hhg_sym_t sym)
{
    hhg_sym_t *new_sym = hhg_arena_malloc(sym_tab->arena, sizeof(hhg_sym_t));
    *new_sym = sym;

    hhg_sym_t **scope = arrlast(sym_tab->tab);

    pshput(scope, new_sym);

    return new_sym;
}

hhg_sym_t *hhg_sym_tab_lookup(hhg_sym_tab_t *sym_tab, const char *key)
{
    size_t len = arrlenu(sym_tab->tab);
    for (size_t i = len; i > 0; i--) {
        hhg_sym_t **scope = sym_tab->tab[i - 1];
        hhg_sym_t *sym = pshget_null(scope, key);
        if (sym != NULL)
            return sym;
    }

    return NULL;
}

void hhg_sym_tab_exit_scope(hhg_sym_tab_t *sym_tab)
{
    hhg_sym_t **scope = arrlast(sym_tab->tab);
    shfree(scope);

    HHG_UNUSED(arrpop(sym_tab->tab));
}

void hhg_sym_tab_del(hhg_sym_tab_t *sym_tab)
{
    hhg_assert(arrlenu(sym_tab->tab) == 0);
    arrfree(sym_tab->tab);
}
