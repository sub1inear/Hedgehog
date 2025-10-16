#include <stdbool.h>

#include <stb_ds.h>

#include "parser.h"
#include "lexer.h"
#include "token.h"
#include "node.h"
#include "type.h"
#include "sym.h"
#include "sym_tab.h"
#include "error.h"
#include "mem.h"

#define hhg_parser_error(...) hhg_error(lexer->pos, lexer->filename, __VA_ARGS__)
#define hhg_parser_warning(...) hhg_warning(lexer->pos, lexer->filename, __VA_ARGS__)
#define hhg_parser_info(...) hhg_info(lexer->pos, lexer->filename, __VA_ARGS__)

static hhg_node_t *hhg_parse_expr(hhg_lexer_t *lexer, int32_t min_prec);
static hhg_node_t *hhg_parse_unary(hhg_lexer_t *lexer);
static void hhg_parse_type(hhg_lexer_t *lexer, hhg_type_t *type);

hhg_node_t *hhg_parse(hhg_lexer_t *lexer)
{
    hhg_node_t *prog = hhg_node_new(HHG_NODE_BLOCK);

    hhg_lexer_next(lexer);

    while (lexer->token.type != EOF) {
        arrput(
            prog->value.block.body,
            hhg_parse_expr(lexer, HHG_PREC_START)
        );
        if (!lexer->newline)
            break;
    }
    hhg_lexer_match(lexer, EOF);
    return prog;
}

static hhg_node_t *hhg_parse_expr(hhg_lexer_t *lexer, int32_t min_prec)
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
    if (hhg_token_is_type(&lexer->token)) {
        hhg_node_t *var_decl = hhg_node_new('=');
        hhg_parse_type(lexer, &var_decl->value_type);

        if (lexer->token.type == HHG_TOKEN_ID)
            var_decl->value.var_decl.id = hhg_strdup(lexer->token.str.str);
        hhg_lexer_match(lexer, HHG_TOKEN_ID);

        hhg_lexer_match(lexer, '=');

        var_decl->value.var_decl.expr =
            hhg_parse_expr(lexer, HHG_PREC_START);
        return var_decl;
    }
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

        while_stmt->value.while_stmt.cond =
            hhg_parse_expr(lexer, HHG_PREC_START);
        while_stmt->value.while_stmt.body =
            hhg_parse_expr(lexer, HHG_PREC_START);

        return while_stmt;
    }
    case HHG_TOKEN_DEF: {
        hhg_lexer_next(lexer);
        hhg_node_t *func_decl = hhg_node_new(HHG_TOKEN_DEF);

        if (lexer->token.type == HHG_TOKEN_ID)
            func_decl->value.func_decl.id = hhg_strdup(lexer->token.str.str);

        hhg_lexer_match(lexer, HHG_TOKEN_ID);
        hhg_lexer_match(lexer, '(');

        while (lexer->token.type != ')') {
            if (lexer->token.type == EOF) {
                hhg_parser_error("unexpected EOF while parsing function args");
                break;
            }
            hhg_node_t *arg = hhg_node_new(HHG_NODE_ARG);

            hhg_parse_type(lexer, &arg->value_type);

            if (lexer->token.type == HHG_TOKEN_ID)
                arg->value.arg.arg = hhg_strdup(lexer->token.str.str);
            hhg_lexer_match(lexer, HHG_TOKEN_ID);

            arrput(func_decl->value.func_decl.args, arg);
            if (lexer->token.type != ')')
                hhg_lexer_match(lexer, ',');
        }
        
        hhg_lexer_match(lexer, ')');
        func_decl->value.func_decl.body =
            hhg_parse_expr(lexer, HHG_PREC_START);
        return func_decl;
    }
    case HHG_TOKEN_RETURN: {
        hhg_lexer_next(lexer);
        hhg_node_t *ret_stmt = hhg_node_new(HHG_TOKEN_RETURN);
        ret_stmt->value.ret.expr = hhg_parse_expr(lexer, HHG_PREC_START);
        return ret_stmt;
    }
    case '{': {
        hhg_node_t *block = hhg_node_new(HHG_NODE_BLOCK);

        hhg_lexer_next(lexer);

        while (lexer->token.type != '}') {
            arrput(
                block->value.block.body,
                hhg_parse_expr(lexer, HHG_PREC_START)
            );
            if (!lexer->newline)
                break;
        }

        hhg_lexer_match(lexer, '}');

        hhg_lexer_next(lexer);

        return block;
    }
    default:
        hhg_fatal_error("invalid syntax");
        return NULL;
    }
}

static void hhg_parse_type(hhg_lexer_t *lexer, hhg_type_t *type)
{
    while (true) {
        // not using switch to break out of loop
        if (lexer->token.type == HHG_TOKEN_CONST) {
            if (type->is_const)
                hhg_parser_error("more than one const in type");
            type->is_const = true;
        } else if (lexer->token.type == HHG_TOKEN_VOLATILE) {
            if (type->is_volatile)
                hhg_parser_error("more than one const in type");
            type->is_volatile = true;
        } else {
            hhg_base_type_t base = hhg_token_type_to_base_type(lexer->token.type);

            if (base == HHG_TYPE_NONE)
                if (type->type == HHG_TYPE_NONE)
                    hhg_parser_error("expected type");
                else
                    break;
            else if (type->type != HHG_TYPE_NONE)
                hhg_parser_error("multiple types");

            type->type = base;
        }
        hhg_lexer_next(lexer);
    }
}
