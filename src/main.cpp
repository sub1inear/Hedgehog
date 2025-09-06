#include "lexer.h"
#include "token.h"
#include "error.h"

using namespace hedgehog;

int main() {
    Lexer lexer { "../../test/testcases/for_range.hhg" };
    while (true) {
        lexer.next();
        if (lexer.token.type == -1) {
            break;
        }
        lexer.token.print();
        putchar('\n');
    }
    return 0;
}