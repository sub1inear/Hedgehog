#include <stdbool.h>

#include "build.h"
#include "lexer.h"
#include "token.h"
#include "parser.h"
#include "node.h"
#include "msg.h"
#include "sym_tab.h"
#include "mem.h"
#include "type_ctx.h"
#include "sem_an.h"
#include "mir.h"

static void hhg_build_debug_print_lexer(hhg_lexer_t *lexer);
static void hhg_build_debug_print_parser(hhg_node_t *prog);
static void hhg_build_debug_print_sem_an(hhg_node_t *prog);
static void hhg_build_debug_print_mir_gen(hhg_mir_gen_t *mir_gen);

bool hhg_build(const char *filename)
{
    return hhg_build_debug(filename, HHG_BUILD_STAGE_BUILD);
}

bool hhg_build_debug(const char *filename, hhg_build_stage_t stage)
{
    // 0th stage: init
    hhg_msg_ctx_t msg_ctx;
    hhg_msg_ctx_init(&msg_ctx);

    hhg_lexer_t lexer;
    hhg_lexer_init(&lexer, &msg_ctx, filename);

    if (stage == HHG_BUILD_STAGE_LEXER) {
        if (msg_ctx.error_count == 0)
            hhg_build_debug_print_lexer(&lexer);

        hhg_build_cleanup(&lexer, NULL, NULL, NULL, NULL);
        return true;
    }

    hhg_arena_t *arena = hhg_arena_new();

    hhg_sym_tab_t sym_tab;
    hhg_sym_tab_init(&sym_tab, arena);
    
    hhg_type_ctx_t type_ctx;
    hhg_type_ctx_init(&type_ctx, arena);

    // 1st stage: lexing, parsing
    hhg_parser_t parser;
    hhg_parser_init(&parser, &lexer, &sym_tab, &type_ctx, &msg_ctx, arena);

    hhg_node_t *prog = hhg_parser_parse(&parser);

    if (stage == HHG_BUILD_STAGE_PARSER) {
        if (msg_ctx.error_count == 0)
            hhg_build_debug_print_parser(prog);

        hhg_build_cleanup(&lexer, &sym_tab, arena, &type_ctx, NULL);
        return msg_ctx.error_count;
    }

    // 2nd stage: semantic analysis
    hhg_sem_an_t sem_an;
    hhg_sem_an_init(&sem_an, &sym_tab, &type_ctx, &msg_ctx, arena);

    hhg_sem_an_run(&sem_an, prog);

    if (stage == HHG_BUILD_STAGE_SEM_AN) {
        if (msg_ctx.error_count == 0)
            hhg_build_debug_print_sem_an(prog);

        hhg_build_cleanup(&lexer, &sym_tab, arena, &type_ctx, NULL);
        return msg_ctx.error_count;
    }

    // 3rd stage: MIR generation
    hhg_mir_gen_t mir_gen;
    hhg_mir_gen_init(&mir_gen, arena);
    hhg_mir_gen_run(&mir_gen, prog);
    if (stage == HHG_BUILD_STAGE_MIR) {
        if (msg_ctx.error_count == 0)
            hhg_build_debug_print_mir_gen(&mir_gen);
        hhg_build_cleanup(&lexer, &sym_tab, arena, &type_ctx, &mir_gen);
        return msg_ctx.error_count;
    }

    // 7th stage: cleanup
    hhg_build_cleanup(&lexer, &sym_tab, arena, &type_ctx, &mir_gen);
    return msg_ctx.error_count;
}

void hhg_build_cleanup(
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

static void hhg_build_debug_print_lexer(hhg_lexer_t *lexer)
{
    // lexer.token.type is initially set to HHG_TOKEN_NONE
    while (lexer->token.type != EOF) {
        hhg_lexer_next(lexer);
        hhg_token_print(&lexer->token);
        putchar('\n');
    }
}
static void hhg_build_debug_print_parser(hhg_node_t *prog)
{
    hhg_node_print(prog, HHG_NODE_INDENT_START, false /* no symbols */);
}

static void hhg_build_debug_print_sem_an(hhg_node_t *prog)
{
    hhg_node_print(prog, HHG_NODE_INDENT_START, true /* print symbols */);
}

static void hhg_build_debug_print_mir_gen(hhg_mir_gen_t *mir_gen)
{
    hhg_mir_gen_print(mir_gen);
}
