#include "debug.h"
#include "lexer.h"
#include "token.h"
#include "parser.h"
#include "node.h"
#include "error.h"
#include "sym_tab.h"
#include "mem.h"
#include "type_ctx.h"
#include "sem_an.h"

void hhg_debug_lexer(const char *filename)
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

bool hhg_debug_parser(const char *filename)
{
    // 0th stage: init
    hhg_lexer_t lexer;
    hhg_lexer_init(&lexer, filename);

    hhg_sym_tab_t sym_tab;
    hhg_sym_tab_init(&sym_tab);

    hhg_arena_t *arena = hhg_arena_new();
    
    hhg_type_ctx_t type_ctx;
    hhg_type_ctx_init(&type_ctx, arena);

    // 1st stage: lexing, parsing
    hhg_parser_t parser;
    hhg_parser_init(&parser, &lexer, &sym_tab, &type_ctx, arena);

    hhg_node_t *prog = hhg_parser_parse(&parser);

    int32_t result = hhg_msgs_get_error_count();

    if (result != 0)
        goto cleanup;

    // 2nd stage: variable analysis
    hhg_sem_an_t var_an;
    hhg_sem_an_init(&var_an, &sym_tab, &type_ctx, arena);

    hhg_sem_an_run(&var_an, prog);

    result = hhg_msgs_get_error_count();
    
    if (result == 0)
        hhg_node_print(prog, HHG_NODE_INDENT_START, true);

    // 3rd stage: type checking

    // 4th stage: memory analysis

    // 5th stage: optimization

    // 6th stage: code generation

    // 7th stage: runtime execution

    // 8th stage: cleanup
    // no cleanup for sem_an needed currently
cleanup:
    hhg_type_ctx_del(&type_ctx);
    hhg_arena_free(arena);
    hhg_sym_tab_del(&sym_tab);
    hhg_lexer_del(&lexer);

    return hhg_msgs_get_error_count();
}
