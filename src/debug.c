#include "debug.h"
#include "lexer.h"
#include "token.h"
#include "parser.h"
#include "node.h"
#include "msg.h"
#include "sym_tab.h"
#include "mem.h"
#include "type_ctx.h"
#include "sem_an.h"

void hhg_debug_lexer(const char *filename)
{
    hhg_msg_ctx_t msg_ctx;
    hhg_msg_ctx_init(&msg_ctx);

    hhg_lexer_t lexer;
    hhg_lexer_init(&lexer, &msg_ctx, filename);

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
    hhg_msg_ctx_t msg_ctx;
    hhg_msg_ctx_init(&msg_ctx);

    hhg_lexer_t lexer;
    hhg_lexer_init(&lexer, &msg_ctx, filename);

    hhg_sym_tab_t sym_tab;
    hhg_sym_tab_init(&sym_tab);

    hhg_arena_t *arena = hhg_arena_new();
    
    hhg_type_ctx_t type_ctx;
    hhg_type_ctx_init(&type_ctx, arena);

    // 1st stage: lexing, parsing
    hhg_parser_t parser;
    hhg_parser_init(&parser, &lexer, &sym_tab, &type_ctx, &msg_ctx, arena);

    hhg_node_t *prog = hhg_parser_parse(&parser);

    if (msg_ctx.error_count != 0)
        goto cleanup;

    // 2nd stage: semantic analysis
    hhg_sem_an_t sem_an;
    hhg_sem_an_init(&sem_an, &sym_tab, &type_ctx, arena);

    hhg_sem_an_run(&sem_an, prog);

    if (msg_ctx.error_count == 0)
        hhg_node_print(prog, HHG_NODE_INDENT_START, true);

    // 3th stage: memory analysis

    // 4th stage: optimization

    // 5th stage: code generation

    // 6th stage: runtime execution

    // 7th stage: cleanup
cleanup:
    // no cleanup for sem_an needed currently
    hhg_type_ctx_del(&type_ctx);
    hhg_arena_free(arena);
    hhg_sym_tab_del(&sym_tab);
    hhg_lexer_del(&lexer);

    return msg_ctx.error_count;
}
