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
#include "mir_gen.h"
#include "code_gen.h"
#include "cfg.h"

typedef struct hhg_build_data {
    hhg_lexer_t *lexer;        // can be NULL
    hhg_sym_tab_t *sym_tab;    // can be NULL
    hhg_type_ctx_t *type_ctx;  // can be NULL
    hhg_mir_gen_t *mir_gen;    // can be NULL
    hhg_code_gen_t *code_gen;  // can be NULL
    hhg_arena_t *arena;        // can be NULL
} hhg_build_data_t;

typedef struct hhg_build_stage_desc {
    hhg_cfg_build_stage_t stage;
    void (*debug_func)(void *);
    void *debug_arg;
} hhg_build_stage_desc_t;

typedef enum hhg_build_check_exit_result_t {
    HHG_BUILD_CHECK_EXIT_SUCCESS,
    HHG_BUILD_CHECK_EXIT_SAFE_EXIT,
    HHG_BUILD_CHECK_EXIT_ERROR,
} hhg_build_check_exit_result_t;
// cleans up build data, handles NULL pointers
static void hhg_build_cleanup(hhg_build_data_t *build_data);
static hhg_build_check_exit_result_t hhg_build_check_exit(
    hhg_cfg_t *cfg,
    hhg_msg_ctx_t *msg_ctx,
    hhg_build_stage_desc_t *stage_desc,
    hhg_build_data_t *build_data
);
static void hhg_build_debug_print_lexer(hhg_lexer_t *lexer);
static void hhg_build_debug_print_parser(hhg_node_t *prog);
static void hhg_build_debug_print_sem_an(hhg_node_t *prog);
static void hhg_build_debug_print_mir_gen(hhg_mir_gen_t *mir_gen);
static void hhg_build_debug_print_code_gen(hhg_code_gen_t *code_gen);

bool hhg_build(hhg_cfg_t *cfg, hhg_arena_t *arena)
{
    // 0th stage: init
    hhg_msg_ctx_t msg_ctx;
    hhg_msg_ctx_init(&msg_ctx, cfg);

    hhg_lexer_t lexer;
    hhg_lexer_init(&lexer, &msg_ctx, cfg->build.entry);

    bool lexer_result = hhg_build_check_exit(
        cfg,
        &msg_ctx,
        &(hhg_build_stage_desc_t) {
            .stage = HHG_CFG_BUILD_STAGE_LEXER,
            .debug_func = hhg_build_debug_print_lexer,
            .debug_arg = &lexer,
        },
        // compound literals set the fields to 0/NULL if unspecified
        &(hhg_build_data_t) {
            .lexer = &lexer,
        }
    );
    if (lexer_result == HHG_BUILD_CHECK_EXIT_ERROR) return true;
    else if (lexer_result == HHG_BUILD_CHECK_EXIT_SAFE_EXIT) return false;

    hhg_sym_tab_t sym_tab;
    hhg_sym_tab_init(&sym_tab, arena);
    
    hhg_type_ctx_t type_ctx;
    hhg_type_ctx_init(&type_ctx, arena);

    // 1st stage: lexing, parsing
    hhg_parser_t parser;
    hhg_parser_init(&parser, &lexer, &sym_tab, &type_ctx, &msg_ctx, arena);

    hhg_node_t *prog = hhg_parser_parse(&parser);

    bool parser_result = hhg_build_check_exit(
        cfg,
        &msg_ctx,
        &(hhg_build_stage_desc_t) {
            .stage = HHG_CFG_BUILD_STAGE_PARSER,
            .debug_func = hhg_build_debug_print_parser,
            .debug_arg = prog,
        },
        &(hhg_build_data_t) {
            .lexer = &lexer,
            .sym_tab = &sym_tab,
        }
    );
    if (parser_result == HHG_BUILD_CHECK_EXIT_ERROR) return true;
    else if (parser_result == HHG_BUILD_CHECK_EXIT_SAFE_EXIT) return false;

    // 2nd stage: semantic analysis
    hhg_sem_an_t sem_an;
    hhg_sem_an_init(&sem_an, &sym_tab, &type_ctx, &msg_ctx, arena);

    hhg_sem_an_run(&sem_an, prog);

    bool sem_an_result = hhg_build_check_exit(
        cfg,
        &msg_ctx,
        &(hhg_build_stage_desc_t) {
            .stage = HHG_CFG_BUILD_STAGE_SEM_AN,
            .debug_func = hhg_build_debug_print_sem_an,
            .debug_arg = prog,
        },
        &(hhg_build_data_t) {
            .lexer = &lexer,
            .sym_tab = &sym_tab,
            .type_ctx = &type_ctx,
        }
    );
    if (sem_an_result == HHG_BUILD_CHECK_EXIT_ERROR) return true;
    else if (sem_an_result == HHG_BUILD_CHECK_EXIT_SAFE_EXIT) return false;

    // 3rd stage: MIR generation
    hhg_mir_gen_t mir_gen;
    hhg_mir_gen_init(&mir_gen, arena);
    hhg_mir_gen_run(&mir_gen, prog);
    
    bool mir_gen_result = hhg_build_check_exit(
        cfg,
        &msg_ctx,
        &(hhg_build_stage_desc_t) {
            .stage = HHG_CFG_BUILD_STAGE_MIR_GEN,
            .debug_func = hhg_build_debug_print_mir_gen,
            .debug_arg = &mir_gen,
        },
        &(hhg_build_data_t) {
            .lexer = &lexer,
            .sym_tab = &sym_tab,
            .type_ctx = &type_ctx,
            .mir_gen = &mir_gen,
        }
    );
    if (mir_gen_result == HHG_BUILD_CHECK_EXIT_ERROR) return true;
    else if (mir_gen_result == HHG_BUILD_CHECK_EXIT_SAFE_EXIT) return false;

    // 4th stage: code generation
    hhg_code_gen_t code_gen;
    hhg_code_gen_backend_t *backend =
        hhg_code_gen_backend_new(arena, cfg->build.backend);
    hhg_code_gen_init(
        &code_gen,
        backend,
        cfg->build.entry,
        cfg->build.out_dir,
        arena
    );

    hhg_code_gen_run(&code_gen, &mir_gen);
    hhg_code_gen_del(&code_gen);
    
    bool code_gen_result = hhg_build_check_exit(
        cfg,
        &msg_ctx,
        &(hhg_build_stage_desc_t) {
              .stage = HHG_CFG_BUILD_STAGE_CODE_GEN,
              .debug_func = hhg_build_debug_print_code_gen,
              .debug_arg = &code_gen,
        },
        &(hhg_build_data_t) {
            .lexer = &lexer,
            .sym_tab = &sym_tab,
            .type_ctx = &type_ctx,
            .mir_gen = &mir_gen,
            .code_gen = &code_gen,
        }
    );
    if (code_gen_result == HHG_BUILD_CHECK_EXIT_ERROR) return true;
    else if (code_gen_result == HHG_BUILD_CHECK_EXIT_SAFE_EXIT) return false;

    // 5th stage: external build


    // 6th stage: cleanup
    hhg_build_cleanup(&(hhg_build_data_t) {
        .lexer = &lexer,
        .sym_tab = &sym_tab,
        .type_ctx = &type_ctx,
        .mir_gen = &mir_gen,
        .code_gen = &code_gen,
    });

    return false;
}

static hhg_build_check_exit_result_t hhg_build_check_exit(
    hhg_cfg_t *cfg,
    hhg_msg_ctx_t *msg_ctx,
    hhg_build_stage_desc_t *stage_desc,
    hhg_build_data_t *build_data
)
{
    if (msg_ctx->error_count > 0) {
        hhg_build_cleanup(build_data);
        return HHG_BUILD_CHECK_EXIT_ERROR;
    }
    if (stage_desc->stage == cfg->build.debug_stage) {
        // safe to print, no errors
        stage_desc->debug_func(stage_desc->debug_arg);
        hhg_build_cleanup(build_data);
        return HHG_BUILD_CHECK_EXIT_SAFE_EXIT;
    }
    if (stage_desc->stage == cfg->build.stage) {
        hhg_build_cleanup(build_data);
        return HHG_BUILD_CHECK_EXIT_SAFE_EXIT;
    }
    return HHG_BUILD_CHECK_EXIT_SUCCESS;
}

void hhg_build_cleanup(hhg_build_data_t *build_data)
{
    if (build_data->mir_gen)  hhg_mir_gen_del(build_data->mir_gen);
    if (build_data->type_ctx) hhg_type_ctx_del(build_data->type_ctx);
    if (build_data->sym_tab)  hhg_sym_tab_del(build_data->sym_tab);
    if (build_data->lexer)    hhg_lexer_del(build_data->lexer);
    if (build_data->code_gen) hhg_code_gen_del(build_data->code_gen);
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

static void hhg_build_debug_print_code_gen(hhg_code_gen_t *code_gen)
{
    hhg_code_gen_backend_print(code_gen->backend);
}
