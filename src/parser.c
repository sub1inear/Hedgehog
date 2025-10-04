#include <stdbool.h>

#include <stb_ds.h>

#include "parser.h"
#include "lexer.h"
#include "node.h"
#include "error.h"
#include "mem.h"

#define hhg_parser_error(...) hhg_error(lexer->pos, lexer->filename, __VA_ARGS__)
#define hhg_parser_warning(...) hhg_warning(lexer->pos, lexer->filename, __VA_ARGS__)
#define hhg_parser_info(...) hhg_info(lexer->pos, lexer->filename, __VA_ARGS__)

hhg_node_t *hhg_parse(hhg_lexer_t *lexer)
{
    hhg_node_t *prog = hhg_node_new(HHG_NODE_BLOCK);

    hhg_lexer_next(lexer);

    while (lexer->token.type != EOF) {
        hhg_lexer_skip(lexer, '\n');

        arrput(
            prog->value.block.body,
            hhg_parse_expr(lexer, HHG_PREC_START)
        );

        if (lexer->token.type == EOF)
            break;

        hhg_lexer_match(lexer, '\n');
    }

    hhg_lexer_next(lexer);

    return prog;
}

hhg_node_t *hhg_parse_expr(hhg_lexer_t *lexer, int32_t min_prec)
{
    hhg_node_t *left = hhg_parse_unary(lexer);
    int32_t prec;
    while (prec = lexer->token.prec,
           prec != HHG_PREC_NONE &&
           prec > min_prec) {
        hhg_node_t *new_left = hhg_node_new(lexer->token.type);
        new_left->value.expr.left = left;

        hhg_lexer_next(lexer);

        new_left->value.expr.right = hhg_parse_expr(lexer, prec);

        left = new_left;
    }

    return left;
}

hhg_node_t *hhg_parse_unary(hhg_lexer_t *lexer)
{
    switch (lexer->token.type) {
    case HHG_TOKEN_ID: {
        char *str = hhg_strdup(lexer->token.str.str);

        hhg_lexer_next(lexer);
        switch (lexer->token.type) {
        case '=': {
            hhg_lexer_next(lexer);
            hhg_node_t *var_decl = hhg_node_new('=');

            var_decl->value.var_decl.id = str;
            var_decl->value.var_decl.expr =
                hhg_parse_expr(lexer, HHG_PREC_START);

            return var_decl;

        }
        default: {
            hhg_node_t *id = hhg_node_new(HHG_TOKEN_ID);
            id->value.id.id = str;
            return id;
        }
        }
    }
    case HHG_TOKEN_STRING_LITERAL:
    case HHG_TOKEN_INT_LITERAL:
    case HHG_TOKEN_FLOAT_LITERAL: {
        hhg_node_t *literal = hhg_node_new(lexer->token.type);
        literal->value.literal.str = hhg_strdup(lexer->token.str.str);
        
        hhg_lexer_next(lexer);
        
        return literal;
    }
    case HHG_TOKEN_IF: {
        hhg_lexer_next(lexer);
        hhg_node_t *if_stmt = hhg_node_new(HHG_TOKEN_IF);

        if_stmt->value.if_stmt.cond = hhg_parse_expr(lexer, HHG_PREC_START);
        if_stmt->value.if_stmt.if_body = hhg_parse_expr(lexer, HHG_PREC_START);

        return if_stmt;
    }
    case HHG_TOKEN_WHILE: {
        hhg_lexer_next(lexer);
        hhg_node_t *while_stmt = hhg_node_new(HHG_TOKEN_WHILE);

        while_stmt->value.while_stmt.cond = hhg_parse_expr(lexer, HHG_PREC_START);
        while_stmt->value.while_stmt.body = hhg_parse_expr(lexer, HHG_PREC_START);

        return while_stmt;
    }
    case '{': {
        hhg_node_t *block = hhg_node_new(HHG_NODE_BLOCK);

        hhg_lexer_next(lexer);

        while (lexer->token.type != '}') {
            hhg_lexer_skip(lexer, '\n');

            if (lexer->token.type == EOF) {
                hhg_parser_error("unexpected EOF while parsing block");
                break;
            }
            
            arrput(
                block->value.block.body,
                hhg_parse_expr(lexer, HHG_PREC_START)
            );

            hhg_lexer_match(lexer, '\n');
        }

        hhg_lexer_next(lexer);

        return block;
    }
    default:
        hhg_fatal_error("invalid syntax");
        return NULL;
    }
}
