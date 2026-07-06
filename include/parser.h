#ifndef HHG_PARSER_H
#define HHG_PARSER_H

#include "lexer.h"
#include "node.h"
#include "sym_tab.h"

typedef struct hhg_parser {
    hhg_lexer_t *lexer;
    hhg_sym_tab_t *sym_tab;
    hhg_type_ctx_t *type_ctx;
    hhg_msg_ctx_t *msg_ctx;
    hhg_arena_t *arena;
} hhg_parser_t;

void hhg_parser_init(hhg_parser_t *parser, hhg_lexer_t *lexer,
                     hhg_sym_tab_t *sym_tab, hhg_type_ctx_t *type_ctx,
                     hhg_msg_ctx_t *msg_ctx, hhg_arena_t *arena);

hhg_node_t *hhg_parser_parse(hhg_parser_t *parser);

#endif
