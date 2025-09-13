#include "lexer.h"
#include "token.h"
#include "error.h"

int main()
{
    hhg_lexer_t lexer;
    hhg_lexer_init(&lexer, "../../test/testcases/print.hhg");
    
    while (true) {
        hhg_lexer_next(&lexer);
        if (lexer.token.type == EOF)
            break;

        hhg_token_print(&lexer.token);
        putchar('\n');
    } 

    hhg_lexer_del(&lexer);

    return 0;
}