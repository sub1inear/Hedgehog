#include <stdbool.h>
#include <time.h>

#include "run.h"
#include "lexer.h"
#include "parser.h"
#include "error.h"
#include "sym_tab.h"

bool hhg_run(const char *filename)
{
    hhg_lexer_t lexer;
    hhg_lexer_init(&lexer, filename);

    hhg_parser_t parser;
    hhg_parser_init(&parser, &lexer);
    hhg_sym_tab_enter_scope();

    hhg_parser_parse(&parser);

    hhg_sym_tab_exit_scope();
    hhg_parser_del(&parser);
    hhg_lexer_del(&lexer);

    return hhg_msgs_get_error_count();
}
