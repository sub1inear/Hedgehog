#ifndef HHG_PARSER_H
#define HHG_PARSER_H

#include "node.h"
#include "lexer.h"
#include "mem.h"
#include "sym_tab.h"

typedef struct hhg_parser {
    hhg_lexer_t *lexer;
    hhg_arena_t *arena;
    hhg_sym_tab_t *sym_tab;
} hhg_parser_t;

void hhg_parser_init(
    hhg_parser_t *parser,
    hhg_lexer_t *lexer,
    hhg_sym_tab_t *sym_tab
);

hhg_node_t *hhg_parser_parse(hhg_parser_t *parser);

void hhg_parser_del(hhg_parser_t *parser);

#endif
