#include <stdbool.h>

#include "run.h"
#include "lexer.h"
#include "parser.h"
#include "error.h"

bool hhg_run(const char *filename)
{
    hhg_lexer_t lexer;
    hhg_lexer_init(&lexer, filename);

    hhg_node_t *program = hhg_parse(&lexer);

    if (hhg_msgs_has_errors()) {
        hhg_msgs_print();
        hhg_msgs_del();
    }

    hhg_lexer_del(&lexer);

    return !hhg_msgs_has_errors();
}