#include <stdbool.h>
#include <string.h>

#include "unit.h"
#include "sym.h"

int main(void)
{
    hhg_sym_init();

    hhg_sym_t result_sym = { "test", { TYPE_I32, false, false, 0 } };

    hhg_sym_insert(&result_sym);

    hhg_sym_t *sym = hhg_sym_lookup("test");

    hhg_unit_assert_msg(sym != NULL, "symbol lookup failed");

    hhg_unit_assert_msg(!strcmp(sym->key, result_sym.key), "restore key failed");
    hhg_unit_assert_msg(sym->value.type == result_sym.value.type, "restore type failed");
    hhg_unit_assert_msg(sym->value.is_const == 1, "restore is_const failed");
    hhg_unit_assert_msg(sym->value.is_volatile == result_sym.value.is_volatile, "restore is_volatile failed");
    hhg_unit_assert_msg(sym->value.ref_count == result_sym.value.ref_count, "restore ref_count failed");

    hhg_sym_del();

    return 0;
}