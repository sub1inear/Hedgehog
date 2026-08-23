#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "mem.h"
#include "sym.h"
#include "sym_tab.h"
#include "unit.h"

int main(void)
{
    hhg_arena_t *arena = hhg_arena_new();

    hhg_sym_tab_t sym_tab;
    hhg_sym_tab_init(&sym_tab, arena);

    hhg_sym_tab_enter_scope(&sym_tab);

    hhg_type_t *type = hhg_type_new(HHG_TYPE_I32, arena);

    hhg_sym_t result_sym = {.key = "test",
                            .value = {
                                .sym_type = HHG_SYM_VAR,
                                .type = type,
                            }};

    hhg_sym_t *sym = hhg_sym_tab_lookup(&sym_tab, "test");

    hhg_unit_assert_msg(sym == NULL, "invalid lookup");

    hhg_sym_tab_insert(&sym_tab, result_sym);

    sym = hhg_sym_tab_lookup(&sym_tab, "test");

    hhg_unit_assert_msg(sym != NULL, "symbol lookup");

    hhg_unit_assert_msg(sym->key == result_sym.key &&
                            sym->value.sym_type == result_sym.value.sym_type &&
                            sym->value.type == result_sym.value.type,
                        "restore symbol");

    hhg_sym_tab_exit_scope(&sym_tab);

    sym = hhg_sym_tab_lookup(&sym_tab, "test");
    hhg_unit_assert_msg(sym == NULL, "exit scope");

    hhg_sym_tab_deinit(&sym_tab);
    hhg_arena_free(arena);

    return EXIT_SUCCESS;
}
