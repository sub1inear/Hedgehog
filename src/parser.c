#include <stdbool.h>

#include <stb_ds.h>

#include "parser.h"
#include "lexer.h"
#include "node.h"
#include "error.h"

#define hhg_parser_error(...) hhg_error(lexer->pos, lexer->filename, __VA_ARGS__)
#define hhg_parser_warning(...) hhg_warning(lexer->pos, lexer->filename, __VA_ARGS__)
#define hhg_parser_info(...) hhg_info(lexer->pos, lexer->filename, __VA_ARGS__)

hhg_node_t *hhg_parse(hhg_lexer_t *lexer)
{
    hhg_node_t *program = hhg_node_new(BLOCK, HHG_STR_EMPTY);

    hhg_lexer_next(lexer);

    while (lexer->token.type != EOF) {
        hhg_lexer_skip(lexer, '\n');

        arrput(program->children, hhg_parse_expr(lexer, HHG_PREC_START));

        if (lexer->token.type == EOF)
            break;

        hhg_lexer_match(lexer, '\n');
    }

    hhg_lexer_next(lexer);

    return program;
}

hhg_node_t *hhg_parse_expr(hhg_lexer_t *lexer, int32_t min_prec)
{
    hhg_node_t *left = hhg_parse_unary(lexer);
    int32_t prec;
    while (prec = lexer->token.prec,
           prec != HHG_PREC_NONE &&
           prec > min_prec) {
        hhg_token_type_t op = lexer->token.type;
        
        hhg_lexer_next(lexer);

        hhg_node_t *right = hhg_parse_expr(lexer, prec);
        left = hhg_node_new_va(op, HHG_STR_EMPTY, 2, left, right);
    }

    return left;
}

hhg_node_t *hhg_parse_unary(hhg_lexer_t *lexer)
{
    switch (lexer->token.type) {
    case ID: {
        hhg_str_t str;
        hhg_str_init_copy(&str, &lexer->token.str);

        hhg_lexer_next(lexer);
        switch (lexer->token.type) {
        case '=': {
            hhg_lexer_next(lexer);
            hhg_node_t *expr = hhg_parse_expr(lexer, HHG_PREC_START);
            return hhg_node_new_va('=', str, 1, expr);
        } 
        default:
            return hhg_node_new(ID, str);
        }
    }
    case INT_LITERAL: {
        hhg_str_t str;
        hhg_str_init_copy(&str, &lexer->token.str);
        hhg_lexer_next(lexer);
        return hhg_node_new(INT_LITERAL, str);
    }
    case '{': {
        hhg_node_t *block = hhg_node_new(BLOCK, HHG_STR_EMPTY);

        hhg_lexer_next(lexer);
        hhg_lexer_skip(lexer, '\n');

        while (lexer->token.type != '}') {
            if (lexer->token.type == EOF) {
                hhg_parser_error("unexpected EOF while parsing block");
                break;
            }
            arrput(block->children, hhg_parse_expr(lexer, HHG_PREC_START));
            hhg_lexer_match(lexer, '\n');
        }

        hhg_lexer_next(lexer);

        return block;
    }
    default:
        hhg_token_print(&lexer->token);
        hhg_fatal_error("unexpected token");
        return NULL;
    }
}