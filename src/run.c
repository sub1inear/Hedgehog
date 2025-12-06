#include <stdbool.h>

#include "run.h"
#include "lexer.h"
#include "parser.h"
#include "error.h"
#include "sym_tab.h"
#include "type_ctx.h"
#include "sem_an.h"
#include "mem.h"

bool hhg_run(const char *filename)
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

    // 2nd stage: semantic analysis
    hhg_sem_an_t sem_an;
    hhg_sem_an_init(&sem_an, &sym_tab, &type_ctx, arena);

    hhg_sem_an_run(&sem_an, prog);

    // 3rd stage: memory analysis

    // 4th stage: optimization

    // 5th stage: code generation

    // 6th stage: runtime execution

    // 7th stage: cleanup
    // no cleanup for sem_an needed currently
cleanup:
    hhg_type_ctx_del(&type_ctx);
    hhg_arena_free(arena);
    hhg_sym_tab_del(&sym_tab);
    hhg_lexer_del(&lexer);

    return hhg_msgs_get_error_count();
}
