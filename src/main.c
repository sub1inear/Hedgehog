#include "lexer.h"
#include "parser.h"

int main()
{
    hhg_lexer_t lexer;
    hhg_lexer_init(&lexer, "../../test/testcases/test.hhg");
    hhg_lexer_next(&lexer);
    
    hhg_node_t *program = hhg_parse(&lexer);

    hhg_node_print(program, HHG_NODE_INDENT_START);

    return 0;
}