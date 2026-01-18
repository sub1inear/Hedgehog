#include <stdbool.h>

#include "run.h"
#include "lexer.h"
#include "parser.h"
#include "msg.h"
#include "sym_tab.h"
#include "type_ctx.h"
#include "sem_an.h"
#include "mir.h"
#include "mem.h"

bool hhg_run(const char *filename)
{
    // 0th stage: init
    hhg_msg_ctx_t msg_ctx;
    hhg_msg_ctx_init(&msg_ctx);

    hhg_lexer_t lexer;
    hhg_lexer_init(&lexer, &msg_ctx, filename);

    hhg_arena_t *arena = hhg_arena_new();

    hhg_sym_tab_t sym_tab;
    hhg_sym_tab_init(&sym_tab, arena);
    
    hhg_type_ctx_t type_ctx;
    hhg_type_ctx_init(&type_ctx, arena);

    // 1st stage: lexing, parsing
    hhg_parser_t parser;
    hhg_parser_init(&parser, &lexer, &sym_tab, &type_ctx, &msg_ctx, arena);

    hhg_node_t *prog = hhg_parser_parse(&parser);

    if (msg_ctx.error_count != 0) {
        hhg_run_cleanup(&lexer, &sym_tab, arena, &type_ctx, NULL);
        return msg_ctx.error_count;
    }

    // 2nd stage: semantic analysis
    hhg_sem_an_t sem_an;
    hhg_sem_an_init(&sem_an, &sym_tab, &type_ctx, &msg_ctx, arena);

    hhg_sem_an_run(&sem_an, prog);

    if (msg_ctx.error_count != 0) {
        hhg_run_cleanup(&lexer, &sym_tab, arena, &type_ctx, NULL);
        return msg_ctx.error_count;
    }

    // 3rd stage: MIR generation
    hhg_mir_gen_t mir_gen;
    hhg_mir_gen_init(&mir_gen, arena);
    hhg_mir_gen_run(&mir_gen, prog);

    // 4th stage: optimization

    // 5th stage: code generation

    // 6th stage: runtime execution

    // 7th stage: cleanup
    hhg_run_cleanup(&lexer, &sym_tab, arena, &type_ctx, &mir_gen);
    return msg_ctx.error_count;
}

void hhg_run_cleanup(
    hhg_lexer_t *lexer,
    hhg_sym_tab_t *sym_tab,
    hhg_arena_t *arena,
    hhg_type_ctx_t *type_ctx,
    hhg_mir_gen_t *mir_gen
)
{
    if (mir_gen) hhg_mir_gen_del(mir_gen);
    if (type_ctx) hhg_type_ctx_del(type_ctx);
    if (arena) hhg_arena_free(arena);
    if (sym_tab) hhg_sym_tab_del(sym_tab);
    hhg_lexer_del(lexer);
}