#ifndef HHG_SYM_TAB_H
#define HHG_SYM_TAB_H

#include "sym.h"

void hhg_sym_tab_enter_scope(void);

hhg_sym_t *hhg_sym_tab_insert(hhg_sym_t sym);
hhg_sym_t *hhg_sym_tab_lookup(const char *key);

void hhg_sym_tab_exit_scope(void);

void hhg_sym_tab_del(void);

#endif
