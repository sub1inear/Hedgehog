#include <stdlib.h>
#include <stdio.h>

#define OPTPARSE_IMPLEMENTATION
#define OPTPARSE_API static
#include <optparse.h>

#include "unit.h"
#include "build.h"
#include "cmd_args.h"
#include "msg.h"
#include "mem.h"

static const struct optparse_long longopts[] = {
    { "help",    'h',  OPTPARSE_NONE },
    { "lexer",   'l',  OPTPARSE_NONE },
    { "parser",  'p',  OPTPARSE_NONE },
    { "sem_an",  's',  OPTPARSE_NONE },
    { "mir",     'm',  OPTPARSE_NONE },
    { "codegen", 'c',  OPTPARSE_NONE },
    { "fail",    'f',  OPTPARSE_NONE },
    { NULL,      '\0', OPTPARSE_NONE },
};

int main(int argc, char **argv)
{
    struct optparse options;
    optparse_init(&options, argv);

    int option;
    hhg_cmd_args_stage_t stop = HHG_CMD_ARGS_STAGE_LEXER;
    bool expect_fail = false;

    while ((option = optparse_long(&options, longopts, NULL)) != -1) {
        switch (option) {
        case 'h':
            printf(
                "usage: %s [-hlpsmcf] <input_file>\n"
                "-h --help    : print this help message\n"
                "-l --lexer   : run up to lexer stage (default)\n"
                "-p --parser  : run up to parser stage\n"
                "-s --sem_an  : run up to semantic analysis stage\n"
                "-m --mir     : run up to MIR generation stage\n"
                "-c --codegen : run up to code generation stage\n"
                "-f --fail    : expect the input file to fail\n",
                argv[0]
            );
            return EXIT_SUCCESS;
        case 'l':
            stop = HHG_CMD_ARGS_STAGE_LEXER;
            break;
        case 'p':
            stop = HHG_CMD_ARGS_STAGE_PARSER;
            break;
        case 's':
            stop = HHG_CMD_ARGS_STAGE_SEM_AN;
            break;
        case 'm':
            stop = HHG_CMD_ARGS_STAGE_MIR_GEN;
            break;
        case 'c':
            stop = HHG_CMD_ARGS_STAGE_CODE_GEN;
            break;
        case 'f':
            expect_fail = true;
            break;
        case '?':
            hhg_fatal_error("%s", options.errmsg);
            break;
        default:
            break;
        }
    }

    char *filename = optparse_arg(&options);
    if (filename == NULL)
        hhg_fatal_error("no input file specified");

    hhg_cmd_args_t cmd_args = {
        .type = HHG_CMD_ARGS_BUILD,
        .subcmd.build = {
            .entry = filename,
            .stop = stop,
            // emit=true so each stage actually runs (e.g. lexer stage drives
            // hhg_lexer_next so lex errors are caught, not just initialization)
            .emit = true,
            .warnings = false,
            .error_warnings = false,
        },
    };

    hhg_msg_ctx_t msg_ctx;
    hhg_msg_ctx_init(&msg_ctx, &cmd_args);

    hhg_arena_t *arena = hhg_arena_new();

    hhg_build(&cmd_args.subcmd.build, &msg_ctx, arena);

    bool failed = (msg_ctx.error_count > 0);

    hhg_arena_free(arena);
    hhg_msg_ctx_del(&msg_ctx);

    if (failed) return expect_fail ? EXIT_SUCCESS : EXIT_FAILURE;
    else        return expect_fail ? EXIT_FAILURE : EXIT_SUCCESS;
}
