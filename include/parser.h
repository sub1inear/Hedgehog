#ifndef HHG_PARSER_H
#define HHG_PARSER_H

#include "node.h"
#include "lexer.h"

#define HHG_PREC_START 0

hhg_node_t *hhg_parse(hhg_lexer_t *lexer);

hhg_node_t *hhg_parse_expr(hhg_lexer_t *lexer, int32_t min_prec);
hhg_node_t *hhg_parse_unary(hhg_lexer_t *lexer);

#endif
