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

    hhg_sym_tab_t sym_tab;
    hhg_sym_tab_init(&sym_tab);

    hhg_parser_t parser;
    hhg_parser_init(&parser, &lexer, &sym_tab);

    hhg_parser_parse(&parser);

    hhg_parser_del(&parser);
    hhg_sym_tab_del(&sym_tab);
    hhg_lexer_del(&lexer);

    return hhg_msgs_get_error_count();
}
