#include <stdlib.h>
#include <stdio.h>

#define OPTPARSE_IMPLEMENTATION
#define OPTPARSE_API static
#include <optparse.h>

#include "unit.h"
#include "debug.h"
#include "msg.h"

static const struct optparse_long longopts[] = {
    { "help",   'h',  OPTPARSE_NONE },
    { "lexer",  'l',  OPTPARSE_NONE },
    { "parser", 'p',  OPTPARSE_NONE },
    { "sem_an", 's',  OPTPARSE_NONE },
    { "fail",   'f',  OPTPARSE_NONE },
    { NULL ,    '\0', OPTPARSE_NONE },
};
int main(int argc, char **argv)
{
    struct optparse options;
    optparse_init(&options, argv);

    int option;
    hhg_stage_t stage = HHG_STAGE_LEXER;
    bool expect_fail = false;
    
    char *prog_name = argv[0][0] == '\0' ? "test_prog" : argv[0];

    while ((option = optparse_long(&options, longopts, NULL)) != -1) {
        switch (option) {
        case 'h':
            printf(
                "usage: %s [-hlpsf] <input_file>\n"
                "-h --help     : print this help message\n"
                "-l --lexer    : debug lexer stage (default)\n"
                "-p --parser   : debug parser stage\n"
                "-s --sem_an   : debug semantic analysis stage\n"
                "-f --fail     : expect the input file to fail\n",
                prog_name
            );
            return EXIT_SUCCESS;
            break;
        case 'l':
            stage = HHG_STAGE_LEXER;
            break;
        case 'p':
            stage = HHG_STAGE_PARSER;
            break;
        case 's':
            stage = HHG_STAGE_SEM_AN;
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

    bool result = hhg_debug(
        filename,
        stage
    );

    if (result) return expect_fail ? EXIT_SUCCESS : EXIT_FAILURE;
    else        return expect_fail ? EXIT_FAILURE : EXIT_SUCCESS;
}
