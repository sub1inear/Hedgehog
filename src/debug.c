#include "debug.h"
#include "lexer.h"
#include "token.h"
#include "parser.h"
#include "node.h"
#include "error.h"

void hhg_debug_lexer(const char *filename)
{
    hhg_lexer_t lexer;
    hhg_lexer_init(&lexer, filename);
    
    while (lexer.token.type != EOF) {
        hhg_lexer_next(&lexer);
        hhg_token_print(&lexer.token);
        putchar('\n');
    }

    hhg_lexer_del(&lexer);
}

bool hhg_debug_parser(const char *filename)
{
    hhg_lexer_t lexer;
    hhg_lexer_init(&lexer, filename);

    hhg_parser_t parser;
    hhg_parser_init(&parser, &lexer);

    hhg_node_t *program = hhg_parser_parse(&parser);

    bool result = hhg_msgs_get_error_count();

    if (result)
        hhg_node_print(program, HHG_NODE_INDENT_START);

    hhg_parser_del(&parser);
    hhg_lexer_del(&lexer);

    return result;
}
