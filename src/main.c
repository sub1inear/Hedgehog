#include "lexer.h"
#include "token.h"
#include "error.h"

int main()
{
    /*hhg_lexer_t lexer;
    hhg_lexer_init(&lexer, "../../test/testcases/print.hhg");
    
    while (true) {
        hhg_lexer_next(&lexer);
        if (lexer.token.type == EOF)
            break;

        hhg_token_print(&lexer.token);
        putchar('\n');
    } 

    hhg_lexer_del(&lexer);*/

    hhg_error("This is an error");
    hhg_warning("This is a warning");
    hhg_info("This is an info");

    hhg_msgs_print();
    hhg_msgs_del();

    return 0;
}