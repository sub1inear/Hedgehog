#include <stdint.h>
#include <assert.h>

#include "sym_tab.h"
#include "sym.h"
#include "mem.h"

#define STB_DS_IMPLEMENTATION
#define STBDS_REALLOC(context, ptr, size) hhg_realloc(ptr, size)
#define STBDS_FREE(context, ptr) hhg_free(ptr)
#include <stb_ds.h>

// unified symbol table
static hhg_sym_t *sym_tab = NULL;
// array of arrays of id strs for keeping track of scope
static const char ***sym_key_arr = NULL;

void hhg_sym_tab_init(void)
{
    sh_new_arena(sym_tab);
}

void hhg_sym_tab_enter_scope(void)
{
    arrput(sym_key_arr, NULL);
}

void hhg_sym_tab_insert(hhg_sym_t *sym)
{
    shput(sym_tab, sym->key, sym->value);
    // pushes sym str onto last entry of sym_key_arr
    size_t last = arrlenu(sym_key_arr) - 1;
    arrput(sym_key_arr[last], sym->key);
}

hhg_sym_t *hhg_sym_tab_lookup(const char *key)
{
    return shgetp_null(sym_tab, key);
}

void hhg_sym_tab_exit_scope(void)
{
    // remove all strs from last entry of sym_key_arr
    size_t outer_len = arrlenu(sym_key_arr);
    if (outer_len > 0) {
        size_t last = outer_len - 1;
        size_t inner_len = arrlenu(sym_key_arr[last]);
        for (size_t i = 0; i < inner_len; i++)
            shdel(sym_tab, sym_key_arr[last][i]);
        arrfree(sym_key_arr[last]);
        arrpop(sym_key_arr);
    }
}

void hhg_sym_tab_del(void)
{
    shfree(sym_tab);
    assert(arrlenu(sym_key_arr) == 0);
    arrfree(sym_key_arr);
}