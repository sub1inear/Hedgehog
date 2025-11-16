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

static hhg_node_t *hhg_parser_parse_expr(
    hhg_parser_t *parser,
    int32_t min_prec
);
static hhg_node_t *hhg_parser_parse_unary(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_var_decl(hhg_parser_t *parser);
static bool hhg_parser_is_type(hhg_parser_t *parser, hhg_token_t *token);
static hhg_type_t hhg_parser_parse_type(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_id(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_bool_literal(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_literal(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_arr_literal(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_if(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_while(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_func_decl(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_return(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_class_decl(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_block(hhg_parser_t *parser);

void hhg_parser_init(
    hhg_parser_t *parser,
    hhg_lexer_t *lexer,
    hhg_sym_tab_t *sym_tab
)
{
    parser->lexer = lexer;
    parser->arena = hhg_arena_new();
    parser->sym_tab = sym_tab;
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
    hhg_sym_tab_clear(parser->sym_tab);
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
    if (hhg_parser_is_type(parser, &parser->lexer->token))
        return hhg_parser_parse_var_decl(parser);
    switch (parser->lexer->token.type) {
    case HHG_TOKEN_ID:
        return hhg_parser_parse_id(parser);
    case HHG_TOKEN_TRUE:
    case HHG_TOKEN_FALSE:
        return hhg_parser_parse_bool_literal(parser);
    case HHG_TOKEN_STRING_LITERAL:
    case HHG_TOKEN_INT_LITERAL:
    case HHG_TOKEN_FLOAT_LITERAL:
        return hhg_parser_parse_literal(parser);
    case '[':
        return hhg_parser_parse_arr_literal(parser);
    case HHG_TOKEN_IF:
        return hhg_parser_parse_if(parser);
    case HHG_TOKEN_WHILE:
        return hhg_parser_parse_while(parser);
    case HHG_TOKEN_DEF:
        return hhg_parser_parse_func_decl(parser);
    case HHG_TOKEN_RETURN:
        return hhg_parser_parse_return(parser);
    case HHG_TOKEN_CLASS:
        return hhg_parser_parse_class_decl(parser);
    case '{':
        return hhg_parser_parse_block(parser);
    default:
        hhg_fatal_error("invalid syntax");
        return NULL;
    }
}

void hhg_parser_del(hhg_parser_t *parser)
{
    hhg_arena_free(parser->arena);
    hhg_sym_tab_del(parser->sym_tab);
}

static bool hhg_parser_is_type(hhg_parser_t *parser, hhg_token_t *token)
{
    if (token->type == HHG_TOKEN_ID) {
        hhg_sym_t *sym = hhg_sym_tab_lookup(
            parser->sym_tab,
            token->str.str
        );
        if (sym == NULL)
            return false;
        switch (sym->value.type) {
        case HHG_TYPE_CLASS:
        case HHG_TYPE_ENUM:
            return true;
        default:
            return false;
        }
    }
    switch (token->type) {
    case HHG_TOKEN_CONST:
    case HHG_TOKEN_VOLATILE:
    case HHG_TOKEN_I8:
    case HHG_TOKEN_U8:
    case HHG_TOKEN_I16:
    case HHG_TOKEN_U16:
    case HHG_TOKEN_I32:
    case HHG_TOKEN_U32:
    case HHG_TOKEN_I64:
    case HHG_TOKEN_U64:
    case HHG_TOKEN_INT:
    case HHG_TOKEN_F32:
    case HHG_TOKEN_F64:
    case HHG_TOKEN_FLOAT:
    case HHG_TOKEN_BOOL:
    case HHG_TOKEN_CHAR:
    case HHG_TOKEN_ISIZE:
    case HHG_TOKEN_USIZE:
    case HHG_TOKEN_TIME_T:
        return true;
    default:
        return false;
    }
}

static hhg_type_t hhg_parser_parse_type(hhg_parser_t *parser)
{
    hhg_type_t type;
    hhg_type_init(&type);

    while (true) {
        // not using switch to break out of loop
        if (parser->lexer->token.type == HHG_TOKEN_CONST) {
            if (type.is_const)
                hhg_parser_error("more than one const in type");
            type.is_const = true;
        } else if (parser->lexer->token.type == HHG_TOKEN_VOLATILE) {
            if (type.is_volatile)
                hhg_parser_error("more than one volatile in type");
            type.is_volatile = true;
        } else if (parser->lexer->token.type == '&') {
            type.type = HHG_TYPE_REF;
            
            hhg_type_t *ref_type = hhg_type_new(parser->arena);
            *ref_type = hhg_parser_parse_type(parser);

            type.info.ref.ref_type = ref_type;
            break; // type is consumed by recursive call
        } else if (parser->lexer->token.type == HHG_TOKEN_ID) {
            hhg_sym_t *sym = hhg_sym_tab_lookup(
                parser->sym_tab,
                parser->lexer->token.str.str
            );
            if (sym == NULL) {
                if (type.type == HHG_TYPE_NONE)
                    hhg_parser_error(
                        "unknown type \"%s\"",
                        parser->lexer->token.str.str
                    );
                else
                    break;
                break;
            }
            
            if (sym->value.type != HHG_TYPE_CLASS &&
                sym->value.type != HHG_TYPE_ENUM) {
                hhg_parser_error(
                    "\"%s\" must be a type",
                    parser->lexer->token.str.str
                );
                break;
            }

            if (type.type != HHG_TYPE_NONE) {
                hhg_parser_error("multiple types");
                break;
            }
            type.type = sym->value.type;
            type.info.sym = sym;
        } else {
            hhg_base_type_t base =
                hhg_token_type_to_base_type(parser->lexer->token.type);

            if (base == HHG_TYPE_NONE) {
                if (type.type == HHG_TYPE_NONE)
                    hhg_parser_error("expected type");
                break;
            } else if (type.type != HHG_TYPE_NONE) {
                hhg_parser_error("multiple types");
                break;
            }

            type.type = base;
        }
        hhg_lexer_next(parser->lexer);
    }
    return type;
}

static hhg_node_t *hhg_parser_parse_var_decl(hhg_parser_t *parser)
{
    hhg_node_t *var_decl = hhg_parser_node_new('=');
    var_decl->value_type = hhg_parser_parse_type(parser);

    if (parser->lexer->token.type == HHG_TOKEN_ID)
        var_decl->value.var_decl.id =
            hhg_parser_strdup(parser->lexer->token.str.str);
    hhg_lexer_match(parser->lexer, HHG_TOKEN_ID);

    hhg_lexer_match(parser->lexer, '=');

    var_decl->value.var_decl.expr =
        hhg_parser_parse_expr(parser, HHG_PREC_START);
    return var_decl;
}

static hhg_node_t *hhg_parser_parse_id(hhg_parser_t *parser)
{
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
    case HHG_TOKEN_INC:
    case HHG_TOKEN_DEC: {
        hhg_node_t *inc_dec = hhg_parser_node_new(parser->lexer->token.type);

        hhg_node_t *id = hhg_parser_node_new(HHG_TOKEN_ID);
        id->value.id.id = str;
        
        inc_dec->value.expr.left = id;
        hhg_lexer_next(parser->lexer);
        return inc_dec;
    }
    case '.': {
        hhg_lexer_next(parser->lexer);
        
        hhg_node_t *field_access = hhg_parser_node_new('.');
        field_access->value.field_access.id = str;

        field_access->value.field_access.next = 
            hhg_parser_parse_unary(parser);

        return field_access;

    }
    default: {
        hhg_node_t *id = hhg_parser_node_new(HHG_TOKEN_ID);
        id->value.id.id = str;
        return id;
    }
    }
}

static hhg_node_t *hhg_parser_parse_bool_literal(hhg_parser_t *parser)
{
    hhg_node_t *literal = hhg_parser_node_new(parser->lexer->token.type);
    hhg_lexer_next(parser->lexer);
    return literal;
}

static hhg_node_t *hhg_parser_parse_literal(hhg_parser_t *parser)
{
    hhg_node_t *literal = hhg_parser_node_new(parser->lexer->token.type);
    literal->value.literal.str =
        hhg_parser_strdup(parser->lexer->token.str.str);

    hhg_lexer_next(parser->lexer);
    return literal;
}

static hhg_node_t *hhg_parser_parse_arr_literal(hhg_parser_t *parser)
{
    hhg_node_t *arr_literal = hhg_parser_node_new(HHG_NODE_ARR_LITERAL);
    hhg_lexer_next(parser->lexer);
    while (parser->lexer->token.type != ']' &&
           parser->lexer->token.type != EOF) {
        arrput(
            arr_literal->value.arr_literal.elems,
            hhg_parser_parse_expr(parser, HHG_PREC_START)
        );
        if (parser->lexer->token.type != ']')
            hhg_lexer_match(parser->lexer, ',');
    }
    hhg_lexer_match(parser->lexer, ']');
    return arr_literal;
}

static hhg_node_t *hhg_parser_parse_if(hhg_parser_t *parser)
{
    hhg_lexer_next(parser->lexer);
    hhg_node_t *if_stmt = hhg_parser_node_new(HHG_TOKEN_IF);

    if_stmt->value.if_stmt.cond =
        hhg_parser_parse_expr(parser, HHG_PREC_START);
    if_stmt->value.if_stmt.if_body =
        hhg_parser_parse_expr(parser, HHG_PREC_START);

    return if_stmt;
}

static hhg_node_t *hhg_parser_parse_while(hhg_parser_t *parser)
{
    hhg_lexer_next(parser->lexer);
    hhg_node_t *while_stmt = hhg_parser_node_new(HHG_TOKEN_WHILE);

    while_stmt->value.while_stmt.cond =
        hhg_parser_parse_expr(parser, HHG_PREC_START);
    while_stmt->value.while_stmt.body =
        hhg_parser_parse_expr(parser, HHG_PREC_START);

    return while_stmt;
}

static hhg_node_t *hhg_parser_parse_func_decl(hhg_parser_t *parser)
{
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

        arg->value_type = hhg_parser_parse_type(parser);

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

static hhg_node_t *hhg_parser_parse_return(hhg_parser_t *parser)
{
    hhg_lexer_next(parser->lexer);
    hhg_node_t *ret_stmt = hhg_parser_node_new(HHG_TOKEN_RETURN);
    ret_stmt->value.ret.expr =
        hhg_parser_parse_expr(parser, HHG_PREC_START);
    return ret_stmt;
}

static hhg_node_t *hhg_parser_parse_class_decl(hhg_parser_t *parser)
{
    hhg_lexer_next(parser->lexer);
    hhg_node_t *class_decl = hhg_parser_node_new(HHG_TOKEN_CLASS);
    if (parser->lexer->token.type == HHG_TOKEN_ID) {
        class_decl->value.class_decl.id =
            hhg_parser_strdup(parser->lexer->token.str.str);
        hhg_sym_tab_insert(
            parser->sym_tab, 
            (hhg_sym_t) {
                .key = class_decl->value.class_decl.id,
                .value = (hhg_type_t) {
                    .type = HHG_TYPE_CLASS,
                },
            }
        );
    }

    hhg_lexer_match(parser->lexer, HHG_TOKEN_ID);
    hhg_lexer_match(parser->lexer, '{');

    while (parser->lexer->token.type != '}' &&
           parser->lexer->token.type != EOF) {
        if (parser->lexer->token.type == HHG_TOKEN_DEF)
            arrput(
                class_decl->value.class_decl.func_decls,
                hhg_parser_parse_func_decl(parser)
            );
        else {
            hhg_node_t *var_decl = hhg_parser_node_new('=');
            var_decl->value_type = hhg_parser_parse_type(parser);
            
            if (var_decl->value_type.type == HHG_TYPE_NONE)
                hhg_parser_error("expected type in class variable declaration");
            
            if (parser->lexer->token.type == HHG_TOKEN_ID)
                var_decl->value.var_decl.id =
                    hhg_parser_strdup(parser->lexer->token.str.str);

            hhg_lexer_match(parser->lexer, HHG_TOKEN_ID);
            arrput(class_decl->value.class_decl.var_decls, var_decl);
        }          
        if (!parser->lexer->newline)
            break;
    }
    hhg_lexer_match(parser->lexer, '}');
    return class_decl;
}

static hhg_node_t *hhg_parser_parse_block(hhg_parser_t *parser)
{
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

    return block;
}