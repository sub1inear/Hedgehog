#include <stdbool.h>

#include "lexer.h"
#include "parser.h"
#include "error.h"
#include "type.h"

int main(void)
{
    /*hhg_lexer_t lexer;
    hhg_lexer_init(&lexer, "../../test/testcases/float.hhg");
    
    while (lexer.token.type != EOF) {
        hhg_lexer_next(&lexer);
        hhg_token_print(&lexer.token);
        putchar('\n');
    }*/
    
    hhg_type_t type = { TYPE_U32, true, true, 2 };
    hhg_type_print(type);
     

    /*hhg_node_t *program = hhg_parse(&lexer);

    if (hhg_msgs_has_errors()) {
        hhg_msgs_print();
        hhg_msgs_del();

        hhg_lexer_del(&lexer);
        return 1;
    }

    hhg_node_print(program, HHG_NODE_INDENT_START);
    hhg_lexer_del(&lexer);
    return 0;*/
}