#include <stdlib.h>
#include <stdio.h>

#define OPTPARSE_IMPLEMENTATION
// declare all optparse functions as static for optimization
#define OPTPARSE_API static
#include <optparse.h>

#include "cmd_args.h"
#include "error.h"

static hhg_cmd_args_t cmd_args;

static const struct optparse_long longopts[] = {
    { "help",     'h',  OPTPARSE_NONE     },
    { "version",  'v',  OPTPARSE_NONE     },
    { "warnings", 'W',  OPTPARSE_NONE     },
    { "heap",     'H',  OPTPARSE_NONE     },
    { "define",   'D',  OPTPARSE_REQUIRED },
    { "run",      'r',  OPTPARSE_REQUIRED },
    { NULL ,      '\0', OPTPARSE_NONE     },
};

void hhg_cmd_args_parse(char **argv)
{
    struct optparse options;
    optparse_init(&options, argv);

    int option;

    char *prog_name = argv[0][0] == '\0' ? "hedgehog" : argv[0];

    while ((option = optparse_long(&options, longopts, NULL)) != -1) {
        switch (option) {
        case 'h':
            printf(
                "usage: %s [-hHvW] [-r cmd] [-D var] [-o dir] [file | -] ...\n"
                "-h --help     : print the help message\n"
                "-H --heap     : print when object escapes to heap\n"
                "-v --version  : print version\n"                   
                "-W --warnings : enable warnings\n"
                "-D --define   : define constexpr bool 'var' to be true\n"
                "-r --run      : compile and run program with 'cmd'\n"
                "file          : file to compile\n"
                "-             : use stdin as file to compile\n"
                "...           : args to pass to --run\n",
                prog_name
            );
            exit(EXIT_SUCCESS);
            break;
        case 'v':
            printf("%s version 0.1\n", prog_name);
            exit(EXIT_SUCCESS);
            break;
        case 'W':
            cmd_args.warnings = true;
            break;
        case 'H':
            cmd_args.heap = true;
            break;
        case 'D':
            break;
        case 'r':
            cmd_args.run_cmd = options.optarg;
            break;
        case '?':
            hhg_fatal_error("%s", options.errmsg);
            break;
        default:
            break;
        }
    }

    cmd_args.filename = optparse_arg(&options);

    cmd_args.pass = &argv[options.optind];

}

hhg_cmd_args_t *hhg_cmd_args_get(void)
{
    return &cmd_args;
}
