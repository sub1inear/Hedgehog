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

#define HHG_PREC_START 0

#define hhg_parser_error(...) \
    hhg_error(parser->lexer->pos, parser->lexer->filename, __VA_ARGS__)
#define hhg_parser_warning(...) \
    hhg_warning(parser->lexer->pos, parser->lexer->filename, __VA_ARGS__)
#define hhg_parser_info(...) \
    hhg_info(parser->lexer->pos, parser->lexer->filename, __VA_ARGS__)
#define hhg_parser_node_new(type) \
    hhg_node_new(parser->arena, type)
#define hhg_parser_strdup(str) \
    hhg_arena_strdup(parser->arena, str)

static hhg_node_t *hhg_parser_parse_expr(hhg_parser_t *parser, int32_t min_prec);
static hhg_node_t *hhg_parser_parse_unary(hhg_parser_t *parser);
static void hhg_parser_parse_type(hhg_parser_t *parser, hhg_type_t *type);

void hhg_parser_init(hhg_parser_t *parser, hhg_lexer_t *lexer)
{
    parser->lexer = lexer;
    parser->arena = hhg_arena_new();
}

hhg_node_t *hhg_parser_parse(hhg_parser_t *parser)
{
    hhg_node_t *prog = hhg_parser_node_new(HHG_NODE_BLOCK);

    hhg_lexer_next(parser->lexer);

    while (parser->lexer->token.type != EOF) {
        arrput(
            prog->value.block.body,
            hhg_parser_parse_expr(parser, HHG_PREC_START)
        );
        if (!parser->lexer->newline)
            break;
    }
    hhg_lexer_match(parser->lexer, EOF);
    return prog;
}

static hhg_node_t *hhg_parser_parse_expr(
    hhg_parser_t *parser,
    int32_t min_prec
)
{
    hhg_node_t *left = hhg_parser_parse_unary(parser);
    int32_t prec;
    while (prec = parser->lexer->token.prec,
           prec != HHG_PREC_NONE &&
           prec > min_prec) {
        hhg_node_t *new_left = hhg_parser_node_new(parser->lexer->token.type);
        new_left->value.expr.left = left;

        hhg_lexer_next(parser->lexer);

        new_left->value.expr.right = hhg_parser_parse_expr(parser, prec);

        left = new_left;
    }

    return left;
}

hhg_node_t *hhg_parser_parse_unary(hhg_parser_t *parser)
{
    if (hhg_token_is_type(&parser->lexer->token)) {
        hhg_node_t *var_decl = hhg_parser_node_new('=');
        hhg_parser_parse_type(parser, &var_decl->value_type);

        if (parser->lexer->token.type == HHG_TOKEN_ID)
            var_decl->value.var_decl.id =
                hhg_parser_strdup(parser->lexer->token.str.str);
        hhg_lexer_match(parser->lexer, HHG_TOKEN_ID);

        hhg_lexer_match(parser->lexer, '=');

        var_decl->value.var_decl.expr =
            hhg_parser_parse_expr(parser, HHG_PREC_START);
        return var_decl;
    }
    switch (parser->lexer->token.type) {
    case HHG_TOKEN_ID: {
        char *str = hhg_parser_strdup(parser->lexer->token.str.str);

        hhg_lexer_next(parser->lexer);
        switch (parser->lexer->token.type) {
        case '=': {
            hhg_lexer_next(parser->lexer);
            hhg_node_t *var_decl = hhg_parser_node_new('=');

            var_decl->value.var_decl.id = str;
            var_decl->value.var_decl.expr =
                hhg_parser_parse_expr(parser, HHG_PREC_START);

            return var_decl;
        }
        case '(': {
            hhg_node_t *func_call = hhg_parser_node_new(HHG_NODE_FUNC_CALL);
            func_call->value.func_call.id = str;
            hhg_lexer_next(parser->lexer);
            while (parser->lexer->token.type != ')' &&
                   parser->lexer->token.type != EOF) {
                arrput(
                    func_call->value.func_call.args,
                    hhg_parser_parse_expr(parser, HHG_PREC_START)
                );
                if (parser->lexer->token.type != ')')
                    hhg_lexer_match(parser->lexer, ',');
            }
            hhg_lexer_match(parser->lexer, ')');
            return func_call;
        }
        default: {
            hhg_node_t *id = hhg_parser_node_new(HHG_TOKEN_ID);
            id->value.id.id = str;
            return id;
        }
        }
    }
    case HHG_TOKEN_TRUE:
    case HHG_TOKEN_FALSE: {
        hhg_node_t *literal = hhg_parser_node_new(parser->lexer->token.type);
        hhg_lexer_next(parser->lexer);
        return literal;
    }
    case HHG_TOKEN_STRING_LITERAL:
    case HHG_TOKEN_INT_LITERAL:
    case HHG_TOKEN_FLOAT_LITERAL: {
        hhg_node_t *literal = hhg_parser_node_new(parser->lexer->token.type);
        literal->value.literal.str =
            hhg_parser_strdup(parser->lexer->token.str.str);

        hhg_lexer_next(parser->lexer);
        return literal;
    }
    case HHG_TOKEN_IF: {
        hhg_lexer_next(parser->lexer);
        hhg_node_t *if_stmt = hhg_parser_node_new(HHG_TOKEN_IF);

        if_stmt->value.if_stmt.cond =
            hhg_parser_parse_expr(parser, HHG_PREC_START);
        if_stmt->value.if_stmt.if_body =
            hhg_parser_parse_expr(parser, HHG_PREC_START);

        return if_stmt;
    }
    case HHG_TOKEN_WHILE: {
        hhg_lexer_next(parser->lexer);
        hhg_node_t *while_stmt = hhg_parser_node_new(HHG_TOKEN_WHILE);

        while_stmt->value.while_stmt.cond =
            hhg_parser_parse_expr(parser, HHG_PREC_START);
        while_stmt->value.while_stmt.body =
            hhg_parser_parse_expr(parser, HHG_PREC_START);

        return while_stmt;
    }
    case HHG_TOKEN_DEF: {
        hhg_lexer_next(parser->lexer);
        hhg_node_t *func_decl = hhg_parser_node_new(HHG_TOKEN_DEF);

        if (parser->lexer->token.type == HHG_TOKEN_ID)
            func_decl->value.func_decl.id =
                hhg_parser_strdup(parser->lexer->token.str.str);

        hhg_lexer_match(parser->lexer, HHG_TOKEN_ID);
        hhg_lexer_match(parser->lexer, '(');

        while (parser->lexer->token.type != ')' &&
               parser->lexer->token.type != EOF) {
            hhg_node_t *arg = hhg_parser_node_new(HHG_NODE_ARG);

            hhg_parser_parse_type(parser, &arg->value_type);

            if (parser->lexer->token.type == HHG_TOKEN_ID)
                arg->value.arg.arg =
                    hhg_parser_strdup(parser->lexer->token.str.str);
            hhg_lexer_match(parser->lexer, HHG_TOKEN_ID);

            arrput(func_decl->value.func_decl.args, arg);
            if (parser->lexer->token.type != ')')
                hhg_lexer_match(parser->lexer, ',');
        }

        hhg_lexer_match(parser->lexer, ')');
        func_decl->value.func_decl.body =
            hhg_parser_parse_expr(parser, HHG_PREC_START);
        return func_decl;
    }
    case HHG_TOKEN_RETURN: {
        hhg_lexer_next(parser->lexer);
        hhg_node_t *ret_stmt = hhg_parser_node_new(HHG_TOKEN_RETURN);
        ret_stmt->value.ret.expr =
            hhg_parser_parse_expr(parser, HHG_PREC_START);
        return ret_stmt;
    }
    case '{': {
        hhg_node_t *block = hhg_parser_node_new(HHG_NODE_BLOCK);

        hhg_lexer_next(parser->lexer);

        while (parser->lexer->token.type != '}') {
            arrput(
                block->value.block.body,
                hhg_parser_parse_expr(parser, HHG_PREC_START)
            );
            if (!parser->lexer->newline)
                break;
        }

        hhg_lexer_match(parser->lexer, '}');

        hhg_lexer_next(parser->lexer);

        return block;
    }
    default:
        hhg_fatal_error("invalid syntax");
        return NULL;
    }
}

void hhg_parser_del(hhg_parser_t *parser)
{
    hhg_arena_free(parser->arena);
}

static void hhg_parser_parse_type(hhg_parser_t *parser, hhg_type_t *type)
{
    while (true) {
        // not using switch to break out of loop
        if (parser->lexer->token.type == HHG_TOKEN_CONST) {
            if (type->is_const)
                hhg_parser_error("more than one const in type");
            type->is_const = true;
        } else if (parser->lexer->token.type == HHG_TOKEN_VOLATILE) {
            if (type->is_volatile)
                hhg_parser_error("more than one volatile in type");
            type->is_volatile = true;
        } else {
            hhg_base_type_t base =
                hhg_token_type_to_base_type(parser->lexer->token.type);

            if (base == HHG_TYPE_NONE)
                if (type->type == HHG_TYPE_NONE)
                    hhg_parser_error("expected type");
                else
                    break;
            else if (type->type != HHG_TYPE_NONE)
                hhg_parser_error("multiple types");

            type->type = base;
        }
        hhg_lexer_next(parser->lexer);
    }
}
