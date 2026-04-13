#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define OPTPARSE_IMPLEMENTATION
// declare all optparse functions as static for optimization
#define OPTPARSE_API static
#include <optparse.h>
#include <stb_ds.h>
#define LIBFS_IMPLEMENTATION
#include <fs.h>

#include "cmd_args.h"
#include "cfg.h"
#include "msg.h"
#include "main.h"
#include "utils.h"

static void hhg_cmd_args_print_global_usage(char *prog_name);

static void hhg_cmd_args_print_init_usage(char *prog_name);
static void hhg_cmd_args_parse_init(
    hhg_cfg_t *cfg,
    char **argv,
    char *prog_name
);

static void hhg_cmd_args_print_build_usage(char *prog_name);
static void hhg_cmd_args_parse_build(
    hhg_cfg_t *cfg,
    char **argv,
    char *prog_name
);

static void hhg_cmd_args_print_run_usage(char *prog_name);
static void hhg_cmd_args_parse_run(
    hhg_cfg_t *cfg,
    char **argv,
    char *prog_name
);

static void hhg_cmd_args_print_test_usage(char *prog_name);
static void hhg_cmd_args_parse_test(
    hhg_cfg_t *cfg,
    char **argv,
    char *prog_name
);

static void hhg_cmd_args_print_clean_usage(char *prog_name);
static void hhg_cmd_args_parse_clean(
    hhg_cfg_t *cfg,
    char **argv,
    char *prog_name
);

static void hhg_cmd_args_print_repl_usage(char *prog_name);
static void hhg_cmd_args_parse_repl(
    hhg_cfg_t *cfg,
    char **argv,
    char *prog_name
);

// note: returns true for NULL (so --color == --color true)
static bool hhg_cmd_args_parse_true_false(const char *str, const char *option);

typedef struct hhg_cmd_args_subcmd_data_t {
    hhg_cmd_args_subcmd_t subcmd;
    const char *str;
} hhg_cmd_args_subcmd_data_t;

void hhg_cmd_args_parse(
    hhg_cfg_t *cfg,
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
        { "color",   'c', OPTPARSE_REQUIRED, },
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
        case 'c':
            cfg->global.color =
                hhg_cmd_args_parse_true_false(global_opts.optarg, "--color");
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

    static const hhg_cmd_args_subcmd_data_t cmd_args_subcmd_data[] = {
        { HHG_CMD_ARGS_SUBCMD_INIT,  "init",  },
        { HHG_CMD_ARGS_SUBCMD_BUILD, "build", },
        { HHG_CMD_ARGS_SUBCMD_RUN,   "run",   },
        { HHG_CMD_ARGS_SUBCMD_TEST,  "test",  },
        { HHG_CMD_ARGS_SUBCMD_CLEAN, "clean", },
        { HHG_CMD_ARGS_SUBCMD_REPL,  "repl",  },
    };
    
    // cfg->subcmd defaults to HHG_CMD_ARGS_SUBCMD_NONE
    for (size_t i = 0; i < HHG_ARR_SIZE(cmd_args_subcmd_data); i++)
        if (strcmp(subcmd_str, cmd_args_subcmd_data[i].str) == 0) {
            cfg->subcmd = cmd_args_subcmd_data[i].subcmd;
            break;
        }

    switch (cfg->subcmd) {
    case HHG_CMD_ARGS_SUBCMD_INIT:
        hhg_cmd_args_parse_init(cfg, subargv, prog_name);
        break;
    case HHG_CMD_ARGS_SUBCMD_BUILD:
        hhg_cmd_args_parse_build(cfg, subargv, prog_name);
        break;
    case HHG_CMD_ARGS_SUBCMD_RUN:
        hhg_cmd_args_parse_run(cfg, subargv, prog_name);
        break;
    case HHG_CMD_ARGS_SUBCMD_TEST:
        hhg_cmd_args_parse_test(cfg, subargv, prog_name);
        break;
    case HHG_CMD_ARGS_SUBCMD_CLEAN:
        hhg_cmd_args_parse_clean(cfg, subargv, prog_name);
        break;
    case HHG_CMD_ARGS_SUBCMD_REPL:
        hhg_cmd_args_parse_repl(cfg, subargv, prog_name);
        break;
    default:
        hhg_fatal_error("unknown subcommand: `%s`", subcmd_str);
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
        "    --color [true|false]   -c    enable or disable color in output\n"
        "subcommands:\n"
        "    init                         create a new Hedgehog project\n"
        "    build                        compile the current project\n"
        "    run                          build and run the project\n"
        "    test                         run tests\n"
        "    clean                        clean build artifacts\n"
        "    repl                         start an interactive Hedgehog shell\n"
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
        "args:\n"
        "    name                   project name (defaults to current directory)\n"
        "options:\n"
        "    --help    -h           show help\n"
        "    --version -v <version> set project version in semver\n"
        "    --std     -s <version> set project standard in semver\n",
        prog_name
    );
}

static void hhg_cmd_args_parse_init(
    hhg_cfg_t *cfg,
    char **argv,
    char *prog_name
)
{
    struct optparse opts;
    optparse_init(&opts, argv);
    static const struct optparse_long longopts[] = {
        { "help",    'h', OPTPARSE_NONE,     },
        { "version", 'v', OPTPARSE_REQUIRED, },
        { "std",     's', OPTPARSE_REQUIRED, },
        { NULL,                              },
    };
    int opt;
    while ((opt = optparse_long(&opts, longopts, NULL)) != -1) {
        switch (opt) {
        case 'h':
            hhg_cmd_args_print_init_usage(prog_name);
            exit(EXIT_SUCCESS);
            break;
        case 'v':
            cfg->init.version = opts.optarg;
            break;
        case 's':
            cfg->init.std = opts.optarg;
            break;
        case '?':
            hhg_fatal_error("%s", opts.errmsg);
            break;
        default:
            break;
        }
    }
    // hhg init will handle NULL as cwd
    cfg->init.name = optparse_arg(&opts);
}

static void hhg_cmd_args_print_build_usage(char *prog_name)
{
    printf(
        "usage: %s build [options]\n"
        "options:\n"
        "    --help                                    -h    show help\n"
        "    --entry <file>                            -e    set entry point\n"
        "    --out-dir <dir>                           -o    set output directory\n"
        "    --mode <debug|release>                    -m    set build mode\n"
        "    --stage <stage>                           -s    stop after specified stage\n"
        "    --debug-stage <stage>                     -S    debug specified stage\n"
        "    --target <triple|auto>                    -t    cross-target triple\n"
        "    --backend <cpp|asm>                       -b    backend format\n"    
        "    --incremental [true|false]                -i    set incremental compilation (only with cpp backend)\n"
        "    --warnings <default|all|none|pedantic>    -W    warnings settings\n"
        "    --error-warnings [true|false]             -E    treat warnings as errors\n"
        "stage:\n"
        "    none|lexer|parser|sem-an|mir-gen|mem-an|code-gen|ext-build\n",
        prog_name
    );
}

static void hhg_cmd_args_parse_build(
    hhg_cfg_t *cfg,
    char **argv,
    char *prog_name
)
{
    struct optparse opts;
    optparse_init(&opts, argv);
    static const struct optparse_long longopts[] = {
        { "help",           'h', OPTPARSE_NONE,     },
        { "entry",          'e', OPTPARSE_REQUIRED, },
        { "out-dir",        'o', OPTPARSE_REQUIRED, },
        { "mode",           'm', OPTPARSE_REQUIRED, },
        { "stage",          's', OPTPARSE_REQUIRED, },
        { "debug-stage",    'S', OPTPARSE_REQUIRED, },
        { "target",         't', OPTPARSE_REQUIRED, },
        { "backend",        'b', OPTPARSE_REQUIRED, },
        { "incremental",    'i', OPTPARSE_OPTIONAL, },
        { "warnings",       'W', OPTPARSE_REQUIRED, },
        { "error-warnings", 'E', OPTPARSE_OPTIONAL, },
        { NULL,                                     },
    };

    int opt;
    while ((opt = optparse_long(&opts, longopts, NULL)) != -1) {
        switch (opt) {
        case 'h':
            hhg_cmd_args_print_build_usage(prog_name);
            exit(EXIT_SUCCESS);
            break;
        case 'e':
            cfg->build.entry = opts.optarg;
            break;
        case 'o':
            cfg->build.out_dir = opts.optarg;
            break;
        case 'm':
            cfg->build.mode = hhg_cfg_parse_build_mode(opts.optarg);
            break;
        case 's':
            cfg->build.stage = hhg_cfg_parse_build_stage(opts.optarg);
            break;
        case 'S':
            cfg->build.debug_stage = hhg_cfg_parse_build_stage(opts.optarg);
            break;
        case 't':
            cfg->build.target = opts.optarg;
            break;
        case 'b':
            cfg->build.backend = hhg_cfg_parse_backend(opts.optarg);
            break;
        case 'i':
            cfg->build.incremental =
                hhg_cmd_args_parse_true_false(opts.optarg, "--incremental");
            break;
        case 'W':
            cfg->build.warnings = hhg_cfg_parse_build_warnings(opts.optarg);
            break;
        case 'E':
            cfg->build.error_warnings =
                hhg_cmd_args_parse_true_false(opts.optarg, "--error-warnings");
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
        "usage: %s run [options] [--] [args]\n"
        "args:\n"
        "    args             arguments to pass to the program\n"
        "options:\n"
        "    --help     -h    show help\n"
        "    --override -O    override config values\n",
        prog_name
    );
}

static void hhg_cmd_args_parse_run(
    hhg_cfg_t *cfg,
    char **argv,
    char *prog_name
)
{
    struct optparse opts;
    optparse_init(&opts, argv);
    static const struct optparse_long longopts[] = {
        { "help",     'h', OPTPARSE_NONE, },
        { "override", 'O', OPTPARSE_NONE, },
        { NULL,                           },
    };
    int opt;
    while ((opt = optparse_long(&opts, longopts, NULL)) != -1) {
        switch (opt) {
        case 'h':
            hhg_cmd_args_print_run_usage(prog_name);
            exit(EXIT_SUCCESS);
            break;
        case 'O':
            // arrfree sets cfg->run.args to NULL
            // so double free is safe
            // even if --override is passed multiple times
            arrfree(cfg->run.args);
            break;
        case '?':
            hhg_fatal_error("%s", opts.errmsg);
            break;
        default:
            break;
        }
    }
    
    char *arg;
    while ((arg = optparse_arg(&opts)) != NULL)
        arrput(cfg->run.args, arg);
}

static void hhg_cmd_args_print_test_usage(char *prog_name)
{
    printf(
        "usage: %s test [options]\n"
        "options:\n"
        "    --help                   -h    show help\n"
        "    --test-dir <dir>         -d    set directory with test cases\n"
        "    --list [true|false]      -l    list test cases without running them\n"
        "    --fail-fast [true|false] -f    stop on first failure\n"
        "    --threads <n>            -j    number of parallel test workers (-1 => auto)\n"
        "    --filter <pattern>       -p    filter test cases (regex)\n",
        prog_name
    );
}

static void hhg_cmd_args_parse_test(
    hhg_cfg_t *cfg,
    char **argv,
    char *prog_name
)
{
    struct optparse opts;
    optparse_init(&opts, argv);
    static const struct optparse_long longopts[] = {
        { "help",      'h', OPTPARSE_NONE,     },
        { "test-dir",  'd', OPTPARSE_REQUIRED, },
        { "list",      'l', OPTPARSE_OPTIONAL, },
        { "fail-fast", 'f', OPTPARSE_OPTIONAL, },
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
        case 'd':
            cfg->test.test_dir = opts.optarg;
            break;
        case 'l':
            cfg->test.list =
                hhg_cmd_args_parse_true_false(opts.optarg, "--list");
            break;
        case 'f':
            cfg->test.fail_fast =
                hhg_cmd_args_parse_true_false(opts.optarg, "--fail-fast");
            break;
        case 'j':
            cfg->test.threads = hhg_utils_str_to_int64(opts.optarg);
            break;
        case 'p':
            cfg->test.filter = opts.optarg;
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
        "    --help                 -h    show help\n"
        "    --mode <all|build|gen> -m    clean mode\n"
        "    --force [true|false]   -f    force clean without confirmation\n"
        "    --dry-run [true|false] -n    print files to be deleted without deleting them\n",
        prog_name
    );
}

static void hhg_cmd_args_parse_clean(
    hhg_cfg_t *cfg,
    char **argv,
    char *prog_name
)
{
    struct optparse opts;
    optparse_init(&opts, argv);
    static const struct optparse_long longopts[] = {
        { "help",    'h', OPTPARSE_NONE,     },
        { "mode",    'm', OPTPARSE_REQUIRED, },
        { "force",   'f', OPTPARSE_OPTIONAL, },
        { "dry-run", 'n', OPTPARSE_OPTIONAL, },
        { NULL,                              },
    };
    int opt;
    while ((opt = optparse_long(&opts, longopts, NULL)) != -1) {
        switch (opt) {
        case 'h':
            hhg_cmd_args_print_clean_usage(prog_name);
            exit(EXIT_SUCCESS);
            break;
        case 'm':
            cfg->clean.mode = hhg_cfg_parse_clean_mode(opts.optarg);
            break;
        case 'f':
            cfg->clean.force =
                hhg_cmd_args_parse_true_false(opts.optarg, "--force");
            break;
        case 'n':
            cfg->clean.dry_run =
                hhg_cmd_args_parse_true_false(opts.optarg, "--dry-run");
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
        "    --help                 -h    show help\n"
        "    --tmp-dir <dir>        -d    set temporary directory for repl\n"
        "    --target <triple|auto> -t    set cross-target triple\n"
        "    --backend <cpp|qbe>    -b    set backend format\n",
        prog_name
    );
}

static void hhg_cmd_args_parse_repl(
    hhg_cfg_t *cfg,
    char **argv,
    char *prog_name
)
{
    struct optparse opts;
    optparse_init(&opts, argv);
    static const struct optparse_long longopts[] = {
        { "help",    'h', OPTPARSE_NONE,     },
        { "tmp-dir", 'd', OPTPARSE_REQUIRED, },
        { "target",  't', OPTPARSE_REQUIRED, },
        { "backend", 'b', OPTPARSE_REQUIRED, },
        { NULL,                              },
    };
    int opt;
    while ((opt = optparse_long(&opts, longopts, NULL)) != -1) {
        switch (opt) {
        case 'h':
            hhg_cmd_args_print_repl_usage(prog_name);
            exit(EXIT_SUCCESS);
            break;
        case 'd':
            cfg->repl.tmp_dir = opts.optarg;
            break;
        case 't':
            cfg->repl.target = opts.optarg;
            break;
        case 'b':
            cfg->repl.backend = hhg_cfg_parse_backend(opts.optarg);
            break;
        case '?':
            hhg_fatal_error("%s", opts.errmsg);
            break;
        default:
            break;
        }
    }
}

static bool hhg_cmd_args_parse_true_false(const char *str, const char *option)
{
    if (str == NULL)
        return true;
    if (strcmp(str, "true") == 0)
        return true;
    if (strcmp(str, "false") == 0)
        return false;
    hhg_fatal_error(
        "invalid value for %s: %s",
        option,
        str
    );
    return true;
}
