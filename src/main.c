#include <stdlib.h>
#include <stdbool.h>

#include "lexer.h"
#include "parser.h"
#include "error.h"
#include "sym.h"
#include "mem.h"

#define STB_DS_IMPLEMENTATION
#define STBDS_REALLOC(context, ptr, size) hhg_realloc(ptr, size)
#define STBDS_FREE(context, ptr) hhg_free(ptr)
#include <stb_ds.h>

static void hhg_test_lexer(const char *filename)
{
    hhg_lexer_t lexer;
    hhg_lexer_init(&lexer, filename);
    
    while (lexer.token.type != EOF) {
        hhg_lexer_next(&lexer);
        hhg_token_print(&lexer.token);
        putchar('\n');
    }

    hhg_lexer_del(&lexer);
}

static void hhg_test_parser(const char *filename)
{
    hhg_lexer_t lexer;
    hhg_lexer_init(&lexer, filename);

    hhg_node_t *program = hhg_parse(&lexer);

    if (hhg_msgs_has_errors()) {
        hhg_msgs_print();
        hhg_msgs_del();

        hhg_lexer_del(&lexer);
        exit(1);
    }

    hhg_node_print(program, HHG_NODE_INDENT_START);
    hhg_lexer_del(&lexer);
}

static void hhg_test_sym_table(void)
{
    hhg_sym_init();

    hhg_sym_insert(&(hhg_sym_t) { "test", { TYPE_I32, false, false, 0 } });

    hhg_type_print(hhg_sym_lookup("test")->value);

    hhg_sym_del();
}

int main(void)
{
    hhg_test_sym_table();
    return 0;
}