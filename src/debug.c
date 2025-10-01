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

void hhg_debug_parser(const char *filename)
{
    hhg_lexer_t lexer;
    hhg_lexer_init(&lexer, filename);

    hhg_node_t *program = hhg_parse(&lexer);

    if (hhg_msgs_get_error_count() > 0) {
        hhg_lexer_del(&lexer);
        return;
    }

    hhg_node_print(program, HHG_NODE_INDENT_START);
    hhg_lexer_del(&lexer);
}
