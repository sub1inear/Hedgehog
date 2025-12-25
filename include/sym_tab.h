#ifndef HHG_SYM_TAB_H
#define HHG_SYM_TAB_H

#include "sym.h"

typedef struct hhg_sym_tab {
    // unified symbol table
    hhg_sym_t **tab;
    // array of arrays of id strs for keeping track of scope
    const char ***key_arr;
    hhg_arena_t *arena;
} hhg_sym_tab_t;

void hhg_sym_tab_init(hhg_sym_tab_t *sym_tab, hhg_arena_t *arena);

void hhg_sym_tab_enter_scope(hhg_sym_tab_t *sym_tab);

hhg_sym_t *hhg_sym_tab_insert(hhg_sym_tab_t *sym_tab, hhg_sym_t sym);

hhg_sym_t *hhg_sym_tab_lookup(hhg_sym_tab_t *sym_tab, const char *key);

void hhg_sym_tab_exit_scope(hhg_sym_tab_t *sym_tab);

void hhg_sym_tab_clear(hhg_sym_tab_t *sym_tab);

void hhg_sym_tab_del(hhg_sym_tab_t *sym_tab);

#endif
