#ifndef HHG_PARSER_H
#define HHG_PARSER_H

#include "node.h"
#include "lexer.h"

typedef struct hhg_lexer hhg_lexer_t;
typedef struct hhg_type_ctx hhg_type_ctx_t;
typedef struct hhg_msg_ctx hhg_msg_ctx_t;
typedef struct arena hhg_arena_t;

typedef struct hhg_parser {
    hhg_lexer_t *lexer;
    hhg_type_ctx_t *type_ctx;
    hhg_msg_ctx_t *msg_ctx;
    hhg_arena_t *arena;
    bool stmt;
} hhg_parser_t;

void hhg_parser_init(
    hhg_parser_t *parser,
    hhg_lexer_t *lexer,
    hhg_type_ctx_t *type_ctx,
    hhg_msg_ctx_t *msg_ctx,
    hhg_arena_t *arena
);

hhg_node_t *hhg_parser_parse(hhg_parser_t *parser);

void hhg_parser_del(hhg_parser_t *parser);

#endif
