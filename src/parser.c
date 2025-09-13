#include "parser.h"
#include "node.h"
#include "lexer.h"

hhg_node_t *hhg_parse(hhg_lexer_t *lexer);

hhg_node_t *hhg_parse_decl(hhg_lexer_t *lexer);
hhg_node_t *hhg_parse_expr(hhg_lexer_t *lexer);
hhg_node_t* hhg_parse_unary(hhg_lexer_t* lexer);