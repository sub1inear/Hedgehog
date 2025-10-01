#include <stdbool.h>
#include <time.h>

#include "run.h"
#include "lexer.h"
#include "parser.h"
#include "error.h"

bool hhg_run(const char *filename)
{
    hhg_lexer_t lexer;
    hhg_lexer_init(&lexer, filename);

    hhg_parse(&lexer);

    hhg_lexer_del(&lexer);

    return hhg_msgs_get_error_count();
}
