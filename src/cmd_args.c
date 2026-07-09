#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define OPTPARSE_IMPLEMENTATION
// declare all optparse functions as static for optimization
#define OPTPARSE_API static
#include <optparse.h>
#include <stb_ds.h>

#include <fs.h>

#include "cmd_args.h"
#include "msg.h"
#include "main.h"
#include "utils.h"

typedef struct hhg_cmd_args_data_t {
    hhg_cmd_args_type_t type;
    const char *str;
} hhg_cmd_args_data_t;

typedef struct hhg_cmd_args_stage_data_t {
    hhg_cmd_args_stage_t stage;
    const char *str;
} hhg_cmd_args_stage_data_t;


static void hhg_cmd_args_print_global_usage(char *prog_name);

static void hhg_cmd_args_print_init_usage(char *prog_name);
static void hhg_cmd_args_parse_init(
    hhg_cmd_args_init_t *init,
    char **argv,
    char *prog_name
);

static void hhg_cmd_args_print_build_usage(char *prog_name);
static struct optparse hhg_cmd_args_parse_build(
    hhg_cmd_args_build_t *build,
    char **argv,
    char *prog_name
);
static hhg_cmd_args_stage_t hhg_cmd_args_parse_stage(const char *str);

static void hhg_cmd_args_print_run_usage(char *prog_name);
static void hhg_cmd_args_parse_run(
    hhg_cmd_args_run_t *run,
    char **argv,
    char *prog_name
);

void hhg_cmd_args_init(
    hhg_cmd_args_t *cmd_args,
    int argc,
    char **argv
)
{
    char *prog_name = argv[0] == NULL ? "hhg" : fs_basename(argv[0]);
    if (argc < 2) {
        hhg_cmd_args_print_global_usage(prog_name);
        exit(EXIT_FAILURE);
    }

    struct optparse global_opts;
    optparse_init(&global_opts, argv);
    
    // disables permutation of non-option arguments
    // so only global options before the subcommand are parsed here
    // e.g. `hhg --help build` will show global help then exit,
    // while `hhg build --help` will parse `build` as subcommand
    // then show build help
    global_opts.permute = 0;

    static const struct optparse_long global_longopts[] = {
        { "help",    'h', OPTPARSE_NONE,     },
        { "version", 'v', OPTPARSE_NONE,     },
        { NULL,                              },
    };

    int opt;

    while ((opt = optparse_long(&global_opts, global_longopts, NULL)) != -1) {
        switch (opt) {
        case 'h':
            hhg_cmd_args_print_global_usage(prog_name);
            exit(EXIT_SUCCESS);
            break;
        case 'v':
            puts(
                "hhg compiler v" HHG_VERSION "\n"
                "copyright (c) 2026- sub1inear"
            );
            exit(EXIT_SUCCESS);
            break;
        case '?':
            hhg_fatal_error("%s", global_opts.errmsg);
            break;
        default:
            break;
        }
    }

    // set before consuming subcommand
    // optparse ignores argv[0]
    char **subargv = argv + global_opts.optind;

    char *subcmd_str = optparse_arg(&global_opts);
    if (subcmd_str == NULL) {
        hhg_cmd_args_print_global_usage(prog_name);
        exit(EXIT_FAILURE);
    }

    static const hhg_cmd_args_data_t cmd_args_subcmd_data[] = {
        { HHG_CMD_ARGS_INIT,  "init",  },
        { HHG_CMD_ARGS_BUILD, "build", },
        { HHG_CMD_ARGS_RUN,   "run",   },
    };
    
    cmd_args->type = HHG_CMD_ARGS_NONE;
    for (size_t i = 0; i < HHG_ARR_LEN(cmd_args_subcmd_data); i++)
        if (strcmp(subcmd_str, cmd_args_subcmd_data[i].str) == 0) {
            cmd_args->type = cmd_args_subcmd_data[i].type;
            break;
        }

    switch (cmd_args->type) {
    case HHG_CMD_ARGS_INIT:
        hhg_cmd_args_parse_init(&cmd_args->subcmd.init, subargv, prog_name);
        break;
    case HHG_CMD_ARGS_BUILD:
        (void)hhg_cmd_args_parse_build(&cmd_args->subcmd.build, subargv, prog_name);
        break;
    case HHG_CMD_ARGS_RUN:
        hhg_cmd_args_parse_run(&cmd_args->subcmd.run, subargv, prog_name);
        break;
    default:
        hhg_fatal_error("unknown subcommand: `%s`", subcmd_str);
        break;
    }
}

void hhg_cmd_args_del(hhg_cmd_args_t *cmd_args)
{
    switch (cmd_args->type) {
    case HHG_CMD_ARGS_INIT:
        break;
    case HHG_CMD_ARGS_BUILD:
        break;
    case HHG_CMD_ARGS_RUN:
        arrfree(cmd_args->subcmd.run.args);
        break;
    default:
        break;
    }
}

static void hhg_cmd_args_print_global_usage(char *prog_name)
{
    printf(
        "usage: %s <subcommand> [options]\n"
        "global options:\n"
        "    --help                 -h    show help\n"
        "    --version              -v    print compiler version\n"
        "subcommands:\n"
        "    init                         create a new Hedgehog project\n"
        "    build                        compile the current project\n"
        "    run                          build and run the project\n"
        "options:\n"
        "    use `%s <subcommand> --help`\n",
        prog_name,
        prog_name
    );
}

static void hhg_cmd_args_print_init_usage(char *prog_name)
{
    printf(
        "usage: %s init [name]\n"
        "args:\n"
        "    name       project name (defaults to current directory)\n",
        prog_name
    );
}

static void hhg_cmd_args_parse_init(
    hhg_cmd_args_init_t *init,
    char **argv,
    char *prog_name
)
{
    HHG_UNUSED(prog_name);
    // hhg init handles NULL as cwd
    init->name = argv[1];
}

static void hhg_cmd_args_print_build_usage(char *prog_name)
{
    printf(
        "usage: %s build entry [options]\n"
        "args:\n"
        "    entry                         entry file to build\n"
        "options:\n"
        "    --help                 -h    show help\n"
        "    --out <path>           -o    set output path\n"
        "    --release              -r    build in release mode\n"
        "    --cxx                  -c    set C++ compiler (default: CXX environment variable)\n"
        "    --stop <stage>         -s    stop after specified stage\n"
        "    --emit                 -e    emit debug output\n"
        "    --warnings             -W    enable warnings\n"
        "    --error-warnings       -E    treat warnings as errors\n"
        "stage:\n"
        "    lexer|parser|sem-an|mir-gen|mem-an|code-gen|ext-build\n",
        prog_name
    );
}

static struct optparse hhg_cmd_args_parse_build(
    hhg_cmd_args_build_t *build,
    char **argv,
    char *prog_name
)
{
    struct optparse opts;
    optparse_init(&opts, argv);
    static const struct optparse_long longopts[] = {
        { "help",           'h', OPTPARSE_NONE,     },
        { "out",            'o', OPTPARSE_REQUIRED, },
        { "release",        'r', OPTPARSE_NONE,     },
        { "stop",           's', OPTPARSE_REQUIRED, },
        { "emit",           'e', OPTPARSE_NONE,     },
        { "warnings",       'W', OPTPARSE_NONE,     },
        { "error-warnings", 'E', OPTPARSE_NONE,     },
        { NULL,                                     },
    };

    *build = (hhg_cmd_args_build_t) {
        .out = NULL,
        .stop = HHG_CMD_ARGS_STAGE_NONE,
        .release = false,
        .emit = false,
        .warnings = false,
        .error_warnings = false,
    };
    
    if ((build->entry = optparse_arg(&opts)) == NULL) {
        hhg_cmd_args_print_build_usage(prog_name);
        exit(EXIT_FAILURE);
    }

    int opt;
    while ((opt = optparse_long(&opts, longopts, NULL)) != -1) {
        switch (opt) {
        case 'h':
            hhg_cmd_args_print_build_usage(prog_name);
            exit(EXIT_SUCCESS);
            break;
        case 'o':
            build->out = opts.optarg;
            break;
        case 'r':
            build->release = true;
            break;
        case 's':
            build->stop = hhg_cmd_args_parse_stage(opts.optarg);
            break;
        case 'W':
            build->warnings = true;
            break;
        case 'E':
            build->error_warnings = true;
            break;
        case '?':
            hhg_fatal_error("%s", opts.errmsg);
            break;
        default:
            break;
        }
    }

    return opts;
}

static hhg_cmd_args_stage_t hhg_cmd_args_parse_stage(const char *str)
{
    hhg_cmd_args_stage_data_t stage_data[] = {
        { HHG_CMD_ARGS_STAGE_LEXER,     "lexer",     },
        { HHG_CMD_ARGS_STAGE_PARSER,    "parser",    },
        { HHG_CMD_ARGS_STAGE_SEM_AN,    "sem-an",    },
        { HHG_CMD_ARGS_STAGE_MIR_GEN,   "mir-gen",   },
        { HHG_CMD_ARGS_STAGE_MEM_AN,    "mem-an",    },
        { HHG_CMD_ARGS_STAGE_CODE_GEN,  "code-gen",  },
        { HHG_CMD_ARGS_STAGE_EXT_BUILD, "ext-build", },
    };
    
    for (size_t i = 0; i < HHG_ARR_LEN(stage_data); i++)
        if (strcmp(str, stage_data[i].str) == 0)
            return stage_data[i].stage;

    hhg_fatal_error("unknown build stage: `%s`", str);
    return HHG_CMD_ARGS_STAGE_NONE;
}


static void hhg_cmd_args_print_run_usage(char *prog_name)
{
    printf(
        "usage: %s build entry [options]\n"
        "args:\n"
        "    entry                         entry file to build\n"
        "options:\n"
        "    --help                 -h    show help\n"
        "    --out <path>           -o    set output path\n"
        "    --release              -r    build in release mode\n"
        "    --cxx                  -c    set C++ compiler (default: CXX environment variable)\n"
        "    --stop <stage>         -s    stop after specified stage\n"
        "    --emit                 -e    emit debug output\n"
        "    --warnings             -W    enable warnings\n"
        "    --error-warnings       -E    treat warnings as errors\n"
        "stage:\n"
        "    lexer|parser|sem-an|mir-gen|mem-an|code-gen|ext-build\n",
        prog_name
    );
}

static void hhg_cmd_args_parse_run(
    hhg_cmd_args_run_t *run,
    char **argv,
    char *prog_name
)
{
    struct optparse opts = hhg_cmd_args_parse_build(&run->build, argv, prog_name);

    run->args = NULL;
    char *arg;
    while ((arg = optparse_arg(&opts)) != NULL)
        arrput(run->args, arg);
}
