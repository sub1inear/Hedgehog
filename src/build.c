#include <stdbool.h>

#include "build.h"
#include "lexer.h"
#include "token.h"
#include "parser.h"
#include "node.h"
#include "msg.h"
#include "sym_tab.h"
#include "type_ctx.h"
#include "sem_an.h"
#include "mir_gen.h"
#include "code_gen.h"
#include "ext_build.h"
#include "cmd_args.h"
#include "utils.h"

typedef struct hhg_build_data {
    hhg_lexer_t *lexer;        // can be NULL
    hhg_sym_tab_t *sym_tab;    // can be NULL
    hhg_type_ctx_t *type_ctx;  // can be NULL
    hhg_mir_gen_t *mir_gen;    // can be NULL
    hhg_code_gen_t *code_gen;  // can be NULL
    hhg_arena_t *arena;        // can be NULL
} hhg_build_data_t;

typedef struct hhg_build_stage_desc {
    hhg_cmd_args_stage_t stage;
    void (*emit_func)(void *);
    void *emit_arg;
} hhg_build_stage_desc_t;

typedef enum hhg_build_check_exit_result_t {
    HHG_BUILD_CHECK_EXIT_SUCCESS,
    HHG_BUILD_CHECK_EXIT_SAFE_EXIT,
    HHG_BUILD_CHECK_EXIT_ERROR,
} hhg_build_check_exit_result_t;
// cleans up build data, handles NULL pointers
static void hhg_build_cleanup(hhg_build_data_t *build_data);
static hhg_build_check_exit_result_t hhg_build_check_exit(
    hhg_cmd_args_build_t *build,
    hhg_msg_ctx_t *msg_ctx,
    hhg_build_stage_desc_t *stage_desc,
    hhg_build_data_t *build_data
);
static void hhg_build_emit_lexer(hhg_lexer_t *lexer);
#if 0 // lexer-only
static void hhg_build_emit_parser(hhg_node_t *prog);
static void hhg_build_emit_sem_an(hhg_node_t *prog);
static void hhg_build_emit_mir_gen(hhg_mir_gen_t *mir_gen);
#endif // lexer-only
static void hhg_build_emit_code_gen(hhg_code_gen_t *code_gen);
static void hhg_build_emit_ext_build(void *arg);

bool hhg_build(
    hhg_cmd_args_build_t *build,
    hhg_msg_ctx_t *msg_ctx,
    hhg_arena_t *arena
)
{
    // 0th stage: init
    hhg_lexer_t lexer;
    hhg_lexer_init(&lexer, msg_ctx, build->entry);

    hhg_build_check_exit_result_t lexer_result = hhg_build_check_exit(
        build,
        msg_ctx,
        &(hhg_build_stage_desc_t) {
            .stage = HHG_CMD_ARGS_STAGE_LEXER,
            .emit_func = hhg_build_emit_lexer,
            .emit_arg = &lexer,
        },
        // compound literals set the fields to 0/NULL if unspecified
        &(hhg_build_data_t) {
            .lexer = &lexer,
        }
    );
    if (lexer_result == HHG_BUILD_CHECK_EXIT_ERROR) return false;
    else if (lexer_result == HHG_BUILD_CHECK_EXIT_SAFE_EXIT) return true;

#if 1
    HHG_UNUSED(arena);
#else // lexer-only: stages 1-5 disabled while token set is in flux
    hhg_sym_tab_t sym_tab;
    hhg_sym_tab_init(&sym_tab, arena);

    hhg_type_ctx_t type_ctx;
    hhg_type_ctx_init(&type_ctx, arena);

    // 1st stage: lexing, parsing
    hhg_parser_t parser;
    hhg_parser_init(&parser, &lexer, &sym_tab, &type_ctx, msg_ctx, arena);

    hhg_node_t *prog = hhg_parser_parse(&parser);

    hhg_build_check_exit_result_t parser_result = hhg_build_check_exit(
        build,
        msg_ctx,
        &(hhg_build_stage_desc_t) {
            .stage = HHG_CMD_ARGS_STAGE_PARSER,
            .emit_func = hhg_build_emit_parser,
            .emit_arg = prog,
        },
        &(hhg_build_data_t) {
            .lexer = &lexer,
            .sym_tab = &sym_tab,
        }
    );
    if (parser_result == HHG_BUILD_CHECK_EXIT_ERROR) return false;
    else if (parser_result == HHG_BUILD_CHECK_EXIT_SAFE_EXIT) return true;

    // 2nd stage: semantic analysis
    hhg_sem_an_t sem_an;
    hhg_sem_an_init(&sem_an, &sym_tab, &type_ctx, msg_ctx, arena);

    hhg_sem_an_run(&sem_an, prog);

    hhg_build_check_exit_result_t sem_an_result = hhg_build_check_exit(
        build,
        msg_ctx,
        &(hhg_build_stage_desc_t) {
            .stage = HHG_CMD_ARGS_STAGE_SEM_AN,
            .emit_func = hhg_build_emit_sem_an,
            .emit_arg = prog,
        },
        &(hhg_build_data_t) {
            .lexer = &lexer,
            .sym_tab = &sym_tab,
            .type_ctx = &type_ctx,
        }
    );
    if (sem_an_result == HHG_BUILD_CHECK_EXIT_ERROR) return false;
    else if (sem_an_result == HHG_BUILD_CHECK_EXIT_SAFE_EXIT) return true;

    // 3rd stage: MIR generation
    hhg_mir_gen_t mir_gen;
    hhg_mir_gen_init(&mir_gen, arena);
    hhg_mir_gen_run(&mir_gen, prog);

    hhg_build_check_exit_result_t mir_gen_result = hhg_build_check_exit(
        build,
        msg_ctx,
        &(hhg_build_stage_desc_t) {
            .stage = HHG_CMD_ARGS_STAGE_MIR_GEN,
            .emit_func = hhg_build_emit_mir_gen,
            .emit_arg = &mir_gen,
        },
        &(hhg_build_data_t) {
            .lexer = &lexer,
            .sym_tab = &sym_tab,
            .type_ctx = &type_ctx,
            .mir_gen = &mir_gen,
        }
    );
    if (mir_gen_result == HHG_BUILD_CHECK_EXIT_ERROR) return false;
    else if (mir_gen_result == HHG_BUILD_CHECK_EXIT_SAFE_EXIT) return true;

    // 4th stage: code generation
    hhg_code_gen_t code_gen;
    hhg_code_gen_init(&code_gen, arena);

    const char *code_gen_filename;
    hhg_code_gen_run(&code_gen, &mir_gen, build->entry, &code_gen_filename);

    hhg_build_check_exit_result_t code_gen_result = hhg_build_check_exit(
        build,
        msg_ctx,
        &(hhg_build_stage_desc_t) {
              .stage = HHG_CMD_ARGS_STAGE_CODE_GEN,
              .emit_func = hhg_build_emit_code_gen,
              .emit_arg = &code_gen,
        },
        &(hhg_build_data_t) {
            .lexer = &lexer,
            .sym_tab = &sym_tab,
            .type_ctx = &type_ctx,
            .mir_gen = &mir_gen,
            .code_gen = &code_gen,
        }
    );
    if (code_gen_result == HHG_BUILD_CHECK_EXIT_ERROR) return false;
    else if (code_gen_result == HHG_BUILD_CHECK_EXIT_SAFE_EXIT) return true;

    // 5th stage: external build
    hhg_ext_build_t ext_build;
    hhg_ext_build_init(&ext_build, msg_ctx, arena);

    hhg_ext_build_run(
        &ext_build,
        &code_gen,
        code_gen_filename,
        build->entry,
        build->out,
        build->release
    );

    hhg_build_check_exit_result_t ext_build_result =
        hhg_build_check_exit(
            build,
            msg_ctx,
            &(hhg_build_stage_desc_t) {
                .stage = HHG_CMD_ARGS_STAGE_EXT_BUILD,
                .emit_func = hhg_build_emit_ext_build,
                .emit_arg = NULL,
            },
            &(hhg_build_data_t) {
                .lexer = &lexer,
                .sym_tab = &sym_tab,
                .type_ctx = &type_ctx,
                .mir_gen = &mir_gen,
                .code_gen = &code_gen,
            }
        );
    if (ext_build_result == HHG_BUILD_CHECK_EXIT_ERROR) return false;
    else if (ext_build_result == HHG_BUILD_CHECK_EXIT_SAFE_EXIT) return true;


    // 6th stage: cleanup
    hhg_build_cleanup(&(hhg_build_data_t) {
        .lexer = &lexer,
        .sym_tab = &sym_tab,
        .type_ctx = &type_ctx,
        .mir_gen = &mir_gen,
        .code_gen = &code_gen,
    });
#endif // lexer-only

    hhg_lexer_del(&lexer);
    return true;
}

static hhg_build_check_exit_result_t hhg_build_check_exit(
    hhg_cmd_args_build_t *build,
    hhg_msg_ctx_t *msg_ctx,
    hhg_build_stage_desc_t *stage_desc,
    hhg_build_data_t *build_data
)
{
    if (msg_ctx->error_count > 0) {
        hhg_build_cleanup(build_data);
        return HHG_BUILD_CHECK_EXIT_ERROR;
    }
    if (stage_desc->stage == build->stop) {
        if (build->emit)
            stage_desc->emit_func(stage_desc->emit_arg);
        hhg_build_cleanup(build_data);
        return HHG_BUILD_CHECK_EXIT_SAFE_EXIT;
    }
    return HHG_BUILD_CHECK_EXIT_SUCCESS;
}

void hhg_build_cleanup(hhg_build_data_t *build_data)
{
#if 0 // lexer-only
    if (build_data->mir_gen)  hhg_mir_gen_del(build_data->mir_gen);
#endif // lexer-only
    if (build_data->type_ctx) hhg_type_ctx_del(build_data->type_ctx);
    if (build_data->sym_tab)  hhg_sym_tab_del(build_data->sym_tab);
    if (build_data->lexer)    hhg_lexer_del(build_data->lexer);
    if (build_data->code_gen) hhg_code_gen_del(build_data->code_gen);
}

static void hhg_build_emit_lexer(hhg_lexer_t *lexer)
{
    do {
        hhg_lexer_next(lexer);
        hhg_token_print(&lexer->token);
        putchar('\n');
    } while (lexer->token.type != HHG_TOKEN_EOF);
}
#if 0 // lexer-only: disabled while token set is in flux
static void hhg_build_emit_parser(hhg_node_t *prog)
{
    hhg_node_print(prog, HHG_NODE_PRINT_MODE_NO_SYM);
}

static void hhg_build_emit_sem_an(hhg_node_t *prog)
{
    hhg_node_print(prog, HHG_NODE_PRINT_MODE_SYM);
}

static void hhg_build_emit_mir_gen(hhg_mir_gen_t *mir_gen)
{
    hhg_mir_gen_print(mir_gen);
}
#endif // lexer-only

static void hhg_build_emit_code_gen(hhg_code_gen_t *code_gen)
{
    HHG_UNUSED(code_gen);
}

static void hhg_build_emit_ext_build(void *arg)
{
    HHG_UNUSED(arg);
}
