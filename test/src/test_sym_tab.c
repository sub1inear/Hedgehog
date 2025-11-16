#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "unit.h"
#include "sym.h"
#include "sym_tab.h"

int main(void)
{
    hhg_sym_tab_t sym_tab;
    hhg_sym_tab_init(&sym_tab);

    hhg_sym_t result_sym;
    result_sym.key = "test";
    hhg_type_init(&result_sym.value);

    hhg_sym_t *sym = hhg_sym_tab_lookup(&sym_tab, "test");

    hhg_unit_assert_msg(sym == NULL, "invalid lookup");

    hhg_sym_tab_insert(&sym_tab, result_sym);

    sym = hhg_sym_tab_lookup(&sym_tab, "test");

    hhg_unit_assert_msg(sym != NULL, "symbol lookup");

    hhg_unit_assert_msg(!strcmp(sym->key, result_sym.key), "restore key");
    hhg_unit_assert_msg(sym->value.type == result_sym.value.type, "restore type");
    hhg_unit_assert_msg(sym->value.is_const == 0, "restore is_const");
    hhg_unit_assert_msg(sym->value.is_volatile == result_sym.value.is_volatile, "restore is_volatile");

    hhg_sym_tab_exit_scope(&sym_tab);

    sym = hhg_sym_tab_lookup(&sym_tab, "test");
    hhg_unit_assert_msg(sym == NULL, "exit scope");

    hhg_sym_tab_del(&sym_tab);

    return EXIT_SUCCESS;
}
