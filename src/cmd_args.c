#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define OPTPARSE_IMPLEMENTATION
// declare all optparse functions as static for optimization
#define OPTPARSE_API static
#include <optparse.h>

#include "cmd_args.h"
#include "msg.h"
#include "utils.h"

static void hhg_cmd_args_print_global_usage(char *prog_name);

static void hhg_cmd_args_print_init_usage(char *prog_name);
static void hhg_cmd_args_parse_init(
    hhg_cmd_args_t *cmd_args,
    char **argv,
    char *prog_name
);

static void hhg_cmd_args_print_build_usage(char *prog_name);
static void hhg_cmd_args_parse_build(
    hhg_cmd_args_t *cmd_args,
    char **argv,
    char *prog_name
);

static void hhg_cmd_args_print_run_usage(char *prog_name);
static void hhg_cmd_args_parse_run(
    hhg_cmd_args_t *cmd_args,
    char **argv,
    char *prog_name
);

static void hhg_cmd_args_print_test_usage(char *prog_name);
static void hhg_cmd_args_parse_test(
    hhg_cmd_args_t *cmd_args,
    char **argv,
    char *prog_name
);

static void hhg_cmd_args_print_clean_usage(char *prog_name);
static void hhg_cmd_args_parse_clean(
    hhg_cmd_args_t *cmd_args,
    char **argv,
    char *prog_name
);

static void hhg_cmd_args_print_repl_usage(char *prog_name);
static void hhg_cmd_args_parse_repl(
    hhg_cmd_args_t *cmd_args,
    char **argv,
    char *prog_name
);

typedef struct hhg_cmd_args_subcmd_data_t {
    hhg_cmd_args_subcmd_type_t subcmd_type;
    const char *str;
} hhg_cmd_args_subcmd_data_t;

hhg_cmd_args_t hhg_cmd_args_parse(int argc, char **argv)
{
    char *prog_name = hhg_utils_path_trunc(argv[0]);

    if (argc < 2) {
        hhg_cmd_args_print_global_usage(prog_name);
        exit(EXIT_FAILURE);
    }

    hhg_cmd_args_t cmd_args = { 0 };

    struct optparse global_opts;
    optparse_init(&global_opts, argv);
    
    // disables permutation of non-option arguments
    // so only global options before the subcommand are parsed here
    // e.g. `hhg --help build` will show global help then exit,
    // while `hhg build --help` will parse `build` as subcommand then show build help
    global_opts.permute = 0;

    static const struct optparse_long global_longopts[] = {
        { "help",    'h', OPTPARSE_NONE, },
        { "version", 'v', OPTPARSE_NONE, },
        { NULL, },
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
                "hhg compiler v0.1\n"
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

    char *subcmd = optparse_arg(&global_opts);
    if (subcmd == NULL) {
        hhg_cmd_args_print_global_usage(prog_name);
        exit(EXIT_FAILURE);
    }

    static const hhg_cmd_args_subcmd_data_t cmd_args_subcmd_data[] = {
        { HHG_CMD_ARGS_SUBCMD_INIT,  "init",  },
        { HHG_CMD_ARGS_SUBCMD_BUILD, "build", },
        { HHG_CMD_ARGS_SUBCMD_RUN,   "run",   },
        { HHG_CMD_ARGS_SUBCMD_TEST,  "test",  },
        { HHG_CMD_ARGS_SUBCMD_CLEAN, "clean", },
        { HHG_CMD_ARGS_SUBCMD_REPL,  "repl",  },
    };

    cmd_args.subcmd_type = HHG_CMD_ARGS_SUBCMD_NONE;
    for (size_t i = 0; i < HHG_ARR_SIZE(cmd_args_subcmd_data); i++)
        if (strcmp(subcmd, cmd_args_subcmd_data[i].str) == 0) {
            cmd_args.subcmd_type = cmd_args_subcmd_data[i].subcmd_type;
            break;
        }

    switch (cmd_args.subcmd_type) {
    case HHG_CMD_ARGS_SUBCMD_INIT:
        hhg_cmd_args_parse_init(&cmd_args, argv, prog_name);
        break;
    case HHG_CMD_ARGS_SUBCMD_BUILD:
        hhg_cmd_args_parse_build(&cmd_args, argv, prog_name);
        break;
    case HHG_CMD_ARGS_SUBCMD_RUN:
        hhg_cmd_args_parse_run(&cmd_args, argv, prog_name);
        break;
    case HHG_CMD_ARGS_SUBCMD_TEST:
        hhg_cmd_args_parse_test(&cmd_args, argv, prog_name);
        break;
    case HHG_CMD_ARGS_SUBCMD_CLEAN:
        hhg_cmd_args_parse_clean(&cmd_args, argv, prog_name);
        break;
    case HHG_CMD_ARGS_SUBCMD_REPL:
        hhg_cmd_args_parse_repl(&cmd_args, argv, prog_name);
        break;
    default:
        hhg_fatal_error("unknown subcommand: `%s`", subcmd);
        break;
    }

    return cmd_args;
}

static void hhg_cmd_args_print_global_usage(char *prog_name)
{
    printf(
        "usage: %s <subcommand> [options]\n"
        "global options:\n"
        "    --help    -h    show help\n"
        "    --version -v    print compiler version\n"
        "subcommands:\n"
        "    init            create a new Hedgehog project\n"
        "    build           compile the current project\n"
        "    run             build and run the project\n"
        "    test            run tests\n"
        "    clean           clean build artifacts\n"
        "    repl            start an interactive Hedgehog shell\n"
        "options:\n"
        "    use `%s <subcommand> --help`\n",
        prog_name,
        prog_name
    );
}

static void hhg_cmd_args_print_init_usage(char *prog_name)
{
    printf(
        "usage: %s init [name] [options]\n"
        "options:\n"
        "    --help -h    show help\n",
        prog_name
    );
}

static void hhg_cmd_args_parse_init(
    hhg_cmd_args_t *cmd_args,
    char **argv,
    char *prog_name
)
{
    HHG_UNUSED(cmd_args);
    struct optparse opts;
    optparse_init(&opts, argv);
    static const struct optparse_long longopts[] = {
        { "help", 'h', OPTPARSE_NONE, },
        { NULL, },
    };
    int opt;
    while ((opt = optparse_long(&opts, longopts, NULL)) != -1) {
        switch (opt) {
        case 'h':
            hhg_cmd_args_print_init_usage(prog_name);
            exit(EXIT_SUCCESS);
            break;
        case '?':
            hhg_fatal_error("%s", opts.errmsg);
            break;
        default:
            break;
        }
    }

    char *proj_name = optparse_arg(&opts);
    if (proj_name == NULL) {
        
    }
}

static void hhg_cmd_args_print_build_usage(char *prog_name)
{
    printf(
        "usage: %s build [options]\n"
        "options:\n"
        "    --help                                                                -h    show help\n"
        "    --debug                                                               -g    debug info (symbols)\n"
        "    --release                                                             -r    release (optimize)\n"
        "    --stage <lexer|parser|sem_an|mir_gen|mem_an|code_gen|ext_build>       -s    stop after specified stage\n"
        "    --debug-stage <lexer|parser|sem_an|mir_gen|mem_an|code_gen|ext_build> -S    debug specified stage\n"
        "    --target <triple>                                                     -t    cross-target triple\n"
        "    --output-target <cpp|asm|auto>                                        -T    output target format (default auto)\n"    
        "    --warnings <..>                                                       -W    none|default|all|pedantic\n"
        "    --deny-warnings                                                       -E    treat warnings as errors\n",
        prog_name
    );
}

static void hhg_cmd_args_parse_build(
    hhg_cmd_args_t *cmd_args,
    char **argv,
    char *prog_name
)
{
    struct optparse opts;
    optparse_init(&opts, argv);
    static const struct optparse_long longopts[] = {
        { "help",          'h', OPTPARSE_NONE,     },
        { "debug",         'g', OPTPARSE_NONE,     },
        { "release",       'r', OPTPARSE_NONE,     },
        { "stage",         's', OPTPARSE_REQUIRED, },
        { "debug-stage",   'S', OPTPARSE_REQUIRED, },
        { "target",        't', OPTPARSE_REQUIRED, },
        { "output-target", 'T', OPTPARSE_REQUIRED, },
        { "warnings",      'W', OPTPARSE_REQUIRED, },
        { "deny-warnings", 'E', OPTPARSE_NONE,     },
        { NULL,                                    },
    };

    int opt;
    while ((opt = optparse_long(&opts, longopts, NULL)) != -1) {
        switch (opt) {
        case 'h':
            hhg_cmd_args_print_build_usage(prog_name);
            exit(EXIT_SUCCESS);
            break;
        case 'g':
            cmd_args->subcmd.build.debug = true;
            break;
        case 'r':
            cmd_args->subcmd.build.release = true;
            break;
        case 's':
            break;
        case 'S':
            break;
        case 't':
            cmd_args->subcmd.build.target = opts.optarg;
            break;
        case 'T':
            break;
        case 'W':
            break;
        case 'E':
            cmd_args->subcmd.build.deny_warnings = true;
            break;
        case '?':
            hhg_fatal_error("%s", opts.errmsg);
            break;
        default:
            break;
        }
    }
}

static void hhg_cmd_args_print_run_usage(char *prog_name)
{
    printf(
        "usage: %s run [args] [options]\n"
        "options:\n"
        "    --help -h    show help\n",
        prog_name
    );
}

static void hhg_cmd_args_parse_run(
    hhg_cmd_args_t *cmd_args,
    char **argv,
    char *prog_name
)
{
    HHG_UNUSED(cmd_args);

    struct optparse opts;
    optparse_init(&opts, argv);
    static const struct optparse_long longopts[] = {
        { "help", 'h', OPTPARSE_NONE, },
        { NULL, },
    };
    int opt;
    while ((opt = optparse_long(&opts, longopts, NULL)) != -1) {
        switch (opt) {
        case 'h':
            hhg_cmd_args_print_run_usage(prog_name);
            exit(EXIT_SUCCESS);
            break;
        case '?':
            hhg_fatal_error("%s", opts.errmsg);
            break;
        default:
            break;
        }
    }
}

static void hhg_cmd_args_print_test_usage(char *prog_name)
{
    printf(
        "usage: %s test [options]\n"
        "options:\n"
        "    --help             -h    show help\n"
        "    --list             -l    list available tests without running\n"
        "    --fail-fast        -f    stop on first failure\n"
        "    --threads <n>      -j    number of parallel test workers (0 => auto)\n"
        "    --filter <pattern> -p    run tests matching pattern (repeatable)\n",
        prog_name
    );
}

static void hhg_cmd_args_parse_test(
    hhg_cmd_args_t *cmd_args,
    char **argv,
    char *prog_name
)
{
    HHG_UNUSED(cmd_args);

    struct optparse opts;
    optparse_init(&opts, argv);
    static const struct optparse_long longopts[] = {
        { "help",      'h', OPTPARSE_NONE,     },
        { "list",      'l', OPTPARSE_NONE,     },
        { "fail-fast", 'f', OPTPARSE_NONE,     },
        { "threads",   'j', OPTPARSE_REQUIRED, },
        { "filter",    'p', OPTPARSE_REQUIRED, },
        { NULL,                                },
    };
    int opt;
    while ((opt = optparse_long(&opts, longopts, NULL)) != -1) {
        switch (opt) {
        case 'h':
            hhg_cmd_args_print_test_usage(prog_name);
            exit(EXIT_SUCCESS);
            break;
        case 'l':
            cmd_args->subcmd.test.list = true;
            break;
        case 'f':
            cmd_args->subcmd.test.fail_fast = true;
            break;
        case 'j':
            break;
        case 'p':
            cmd_args->subcmd.test.pattern = opts.optarg;
            break;
        case '?':
            hhg_fatal_error("%s", opts.errmsg);
            break;
        default:
            break;
        }
    }
}

static void hhg_cmd_args_print_clean_usage(char *prog_name)
{
    printf(
        "usage: %s clean [options]\n"
        "options:\n"
        "    --help -h    show help\n",
        prog_name
    );
}

static void hhg_cmd_args_parse_clean(
    hhg_cmd_args_t *cmd_args,
    char **argv,
    char *prog_name
)
{
    HHG_UNUSED(cmd_args);

    struct optparse opts;
    optparse_init(&opts, argv);
    static const struct optparse_long longopts[] = {
        { "help", 'h', OPTPARSE_NONE, },
        { NULL,                       },
    };
    int opt;
    while ((opt = optparse_long(&opts, longopts, NULL)) != -1) {
        switch (opt) {
        case 'h':
            hhg_cmd_args_print_clean_usage(prog_name);
            exit(EXIT_SUCCESS);
            break;
        case '?':
            hhg_fatal_error("%s", opts.errmsg);
            break;
        default:
            break;
        }
    }
}

static void hhg_cmd_args_print_repl_usage(char *prog_name)
{
    printf(
        "usage: %s repl [options]\n"
        "options:\n"
        "    --help -h    show help\n",
        prog_name
    );
}

static void hhg_cmd_args_parse_repl(
    hhg_cmd_args_t *cmd_args,
    char **argv,
    char *prog_name
)
{
    HHG_UNUSED(cmd_args);

    struct optparse opts;
    optparse_init(&opts, argv);
    static const struct optparse_long longopts[] = {
        { "help", 'h', OPTPARSE_NONE, },
        { NULL,                       },
    };
    int opt;
    while ((opt = optparse_long(&opts, longopts, NULL)) != -1) {
        switch (opt) {
        case 'h':
            hhg_cmd_args_print_repl_usage(prog_name);
            exit(EXIT_SUCCESS);
            break;
        case '?':
            hhg_fatal_error("%s", opts.errmsg);
            break;
        default:
            break;
        }
    }
}
