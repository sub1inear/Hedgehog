#include <stddef.h> // for NULL
#include <stdbool.h>
#include <assert.h>

#include <stb_ds.h>

#include "parser.h"
#include "lexer.h"
#include "token.h"
#include "node.h"
#include "type.h"
#include "type_ctx.h"
#include "sym.h"
#include "sym_tab.h"
#include "msg.h"
#include "mem.h"
#include "file_pos.h"
#include "file_range.h"
#include "utils.h"

#define HHG_PREC_START 0

#define hhg_parser_msg(parser, type, ...) \
    hhg_msg(                              \
        parser->msg_ctx,                  \
        type,                             \
        &parser->lexer->src,              \
        &parser->lexer->token.range,      \
        __VA_ARGS__                       \
    )

#define hhg_parser_error(parser, ...) \
    hhg_parser_msg(parser, HHG_MSG_ERROR, __VA_ARGS__)

#define hhg_parser_warning(parser, range, ...) \
    hhg_parser_msg(parser, HHG_MSG_WARNING, __VA_ARGS__)

#define hhg_parser_info(parser, range, ...) \
    hhg_parser_msg(parser, HHG_MSG_INFO, range, __VA_ARGS__)

typedef struct hhg_parser_cv_qual {
    bool is_const;
    bool is_volatile;
} hhg_parser_cv_qual_t;

static hhg_node_t *hhg_parser_parse_expr(
    hhg_parser_t *parser,
    int32_t min_prec
);
static hhg_node_t *hhg_parser_parse_unary(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_unary_core(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_var_decl(
    hhg_parser_t *parser,
    hhg_type_t *type
);
static hhg_node_t *hhg_parser_parse_obj_init(
    hhg_parser_t *parser,
    hhg_type_t *type
);
static bool hhg_parser_is_type(hhg_parser_t *parser, hhg_token_t *token);
static hhg_type_t *hhg_parser_parse_type(hhg_parser_t *parser);
static hhg_parser_cv_qual_t hhg_parser_parse_cv_qual(hhg_parser_t *parser);
static hhg_type_t *hhg_parser_parse_base_type(hhg_parser_t *parser);
static hhg_type_t *hhg_parser_parse_ref_type(
    hhg_parser_t *parser,
    hhg_type_t *base
);
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

static hhg_node_t *hhg_parser_node_new(
    hhg_parser_t *parser,
    hhg_node_type_t type
);
static char *hhg_parser_strdup(hhg_parser_t *parser, const char *str);

void hhg_parser_init(
    hhg_parser_t *parser,
    hhg_lexer_t *lexer,
    hhg_sym_tab_t *sym_tab,
    hhg_type_ctx_t *type_ctx,
    hhg_msg_ctx_t *msg_ctx,
    hhg_arena_t *arena
)
{
    *parser = (hhg_parser_t) {
        .lexer = lexer,
        .sym_tab = sym_tab,
        .type_ctx = type_ctx,
        .msg_ctx = msg_ctx,
        .arena = arena
    };
}

hhg_node_t *hhg_parser_parse(hhg_parser_t *parser)
{
    hhg_node_t *prog = hhg_parser_node_new(parser, HHG_NODE_BLOCK);

    hhg_sym_tab_enter_scope(parser->sym_tab);

    hhg_lexer_next(parser->lexer);

    hhg_file_pos_t start_pos = parser->lexer->token.range.start;

    while (parser->lexer->token.type != EOF) {
        arrput(
            prog->value.block.body,
            hhg_parser_parse_expr(parser, HHG_PREC_START)
        );
        if (!parser->lexer->newline)
            break;
    }
    hhg_lexer_match(parser->lexer, EOF);

    prog->range = (hhg_file_range_t) {
        .start = start_pos,
        .end = parser->lexer->token.range.end
    };

    hhg_sym_tab_exit_scope(parser->sym_tab);
    hhg_sym_tab_clear(parser->sym_tab);
    return prog;
}

// Pratt Parser implementation
// hard to understand from code,
// see https://martin.janiczek.cz/2023/07/03/demystifying-pratt-parsers.html
// to step through the algorithm
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
        hhg_node_t *new_left =
            hhg_parser_node_new(parser, parser->lexer->token.type);
        new_left->value.expr.left = left;

        hhg_lexer_next(parser->lexer);

        new_left->value.expr.right = hhg_parser_parse_expr(parser, prec);

        new_left->range = (hhg_file_range_t) {
            .start = left->range.start,
            .end = new_left->value.expr.right->range.end
        };

        left = new_left;

    }

    return left;
}

static hhg_node_t *hhg_parser_parse_unary(hhg_parser_t *parser)
{
    hhg_file_pos_t start_pos = parser->lexer->token.range.start;
    hhg_node_t *node = hhg_parser_parse_unary_core(parser);
    node->range = (hhg_file_range_t) {
        .start = start_pos,
        .end = parser->lexer->last_pos
    };
    return node;
}

static hhg_node_t *hhg_parser_parse_unary_core(hhg_parser_t *parser) {
    if (hhg_parser_is_type(parser, &parser->lexer->token)) {
        hhg_type_t *type = hhg_parser_parse_type(parser);
        switch (parser->lexer->token.type) {
        case HHG_TOKEN_ID:
            return hhg_parser_parse_var_decl(parser, type);
        case '(':
            return hhg_parser_parse_obj_init(parser, type);
        default:
            hhg_parser_error(parser, "invalid syntax after type", "here");
            return hhg_parser_node_new(parser, HHG_TOKEN_NONE);
        }
    }
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
        hhg_parser_error(parser, "invalid syntax", "here");
        return hhg_parser_node_new(parser, HHG_TOKEN_NONE);
    }
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
        switch (sym->value.sym_type) {
        case HHG_SYM_CLASS:
        case HHG_SYM_ENUM:
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
        return true;
    default:
        return false;
    }
}

static hhg_type_t *hhg_parser_parse_type(hhg_parser_t *parser)
{
    hhg_parser_cv_qual_t cv_qual = hhg_parser_parse_cv_qual(parser);

    hhg_type_t *type = hhg_parser_parse_base_type(parser);

    if ((cv_qual.is_const || cv_qual.is_volatile) && type != NULL) {
        type = hhg_type_ctx_new_cv_type(
            parser->type_ctx,
            (hhg_cv_tab_key_t) {
                .base = type,
                .is_const = cv_qual.is_const,
                .is_volatile = cv_qual.is_volatile,
            }
        );
    }

    hhg_lexer_next(parser->lexer);
    return type;
}

static hhg_parser_cv_qual_t hhg_parser_parse_cv_qual(hhg_parser_t *parser)
{
    hhg_parser_cv_qual_t cv_qual = {
        .is_const = false,
        .is_volatile = false
    };
    while (true) {
        // not using switch to break out of loop
        if (parser->lexer->token.type == HHG_TOKEN_CONST) {
            if (cv_qual.is_const)
                hhg_parser_error(
                    parser,
                    "more than one const in type",
                    "duplicate const here"
                );
            cv_qual.is_const = true;
        } else if (parser->lexer->token.type == HHG_TOKEN_VOLATILE) {
            if (cv_qual.is_volatile)
                hhg_parser_error(
                    parser,
                    "more than one volatile in type",
                    "duplicate volatile here"
                );
            cv_qual.is_volatile = true;
        } else
            break;
        hhg_lexer_next(parser->lexer);
    }
    return cv_qual;
}

static hhg_type_t *hhg_parser_parse_base_type(hhg_parser_t *parser)
{
    hhg_type_t *type = NULL;
    if (parser->lexer->token.type == HHG_TOKEN_ID) {
        hhg_sym_t *sym = hhg_sym_tab_lookup(
            parser->sym_tab,
            parser->lexer->token.str.str
        );

        if (sym == NULL) {
            hhg_parser_error(
                parser,
                "unknown type `%s`",
                "`%s` used here",
                parser->lexer->token.str.str
            );
            return NULL;
        }

        if (sym->value.sym_type != HHG_SYM_CLASS &&
            sym->value.sym_type != HHG_SYM_ENUM) {
            hhg_parser_error(
                parser,
                "`%s` must be a type",
                "`%s` used here",
                parser->lexer->token.str.str
            );
            return NULL;
        }
        type = sym->value.type;
    } else {
        hhg_base_type_t base =
            hhg_token_type_to_base_type(parser->lexer->token.type);

        if (base == HHG_TYPE_NONE) {
            hhg_parser_error(
                parser,
                "expected type, got `%s`",
                "here",
                parser->lexer->token.str.str
            );
            return NULL;
        }

        type = hhg_type_ctx_get_builtin(
            parser->type_ctx,
            base
        );
    }
    hhg_assert(type != NULL);
    return type;
}
static hhg_type_t *hhg_parser_parse_ref_type(
    hhg_parser_t *parser,
    hhg_type_t *base
)
{
    while (parser->lexer->token.type == '&') {
        hhg_parser_cv_qual_t cv_qual = hhg_parser_parse_cv_qual(parser);

        base = hhg_type_ctx_new_ref(
            parser->type_ctx,
            (hhg_ref_tab_key_t) {
                .base = base,
                .is_const = cv_qual.is_const,
                .is_volatile = cv_qual.is_volatile,
            }
        );

        hhg_lexer_next(parser->lexer);
    }
    return base;
}

static hhg_node_t *hhg_parser_parse_var_decl(
    hhg_parser_t *parser,
    hhg_type_t *type
)
{
    hhg_node_t *var_decl = hhg_parser_node_new(parser, '=');
    var_decl->value_type = type;

    if (parser->lexer->token.type == HHG_TOKEN_ID)
        var_decl->value.var_decl.id.str =
            hhg_parser_strdup(parser, parser->lexer->token.str.str);
    hhg_lexer_match(parser->lexer, HHG_TOKEN_ID);

    hhg_lexer_match(parser->lexer, '=');

    var_decl->value.var_decl.expr =
        hhg_parser_parse_expr(parser, HHG_PREC_START);
    return var_decl;
}

static hhg_node_t *hhg_parser_parse_obj_init(
    hhg_parser_t *parser,
    hhg_type_t *type
)
{
    hhg_lexer_next(parser->lexer);

    hhg_node_t *obj_init = hhg_parser_node_new(parser, HHG_NODE_OBJ_INIT);
    obj_init->value_type = type;

    while (parser->lexer->token.type != ')' &&
           parser->lexer->token.type != EOF) {
        arrput(
            obj_init->value.obj_init.args,
            hhg_parser_parse_expr(parser, HHG_PREC_START)
        );
        if (parser->lexer->token.type != ')')
            hhg_lexer_match(parser->lexer, ',');
    }
    hhg_lexer_match(parser->lexer, ')');
    return obj_init;
}

static hhg_node_t *hhg_parser_parse_id(hhg_parser_t *parser)
{
    char *str = hhg_parser_strdup(parser, parser->lexer->token.str.str);

    hhg_lexer_next(parser->lexer);
    switch (parser->lexer->token.type) {
    case '=': {
        hhg_lexer_next(parser->lexer);
        hhg_node_t *var_decl = hhg_parser_node_new(parser, '=');

        var_decl->value.var_decl.id.str = str;
        var_decl->value.var_decl.expr =
            hhg_parser_parse_expr(parser, HHG_PREC_START);

        return var_decl;
    }
    case '(': {
        hhg_node_t *func_call =
            hhg_parser_node_new(parser, HHG_NODE_FUNC_CALL);
        func_call->value.func_call.id.str = str;
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
        hhg_node_t *inc_dec =
            hhg_parser_node_new(parser, parser->lexer->token.type);

        hhg_node_t *id = hhg_parser_node_new(parser, HHG_TOKEN_ID);
        id->value.id.str = str;

        inc_dec->value.expr.left = id;
        hhg_lexer_next(parser->lexer);
        return inc_dec;
    }
    case '.': {
        hhg_lexer_next(parser->lexer);

        hhg_node_t *field_access = hhg_parser_node_new(parser, '.');
        field_access->value.field_access.str = str;

        field_access->value.field_access.next =
            hhg_parser_parse_unary(parser);

        return field_access;

    }
    default: {
        hhg_node_t *id = hhg_parser_node_new(parser, HHG_TOKEN_ID);
        id->value.id.str = str;
        return id;
    }
    }
}

static hhg_node_t *hhg_parser_parse_bool_literal(hhg_parser_t *parser)
{
    hhg_node_t *literal =
        hhg_parser_node_new(parser, parser->lexer->token.type);
    hhg_lexer_next(parser->lexer);
    return literal;
}

static hhg_node_t *hhg_parser_parse_literal(hhg_parser_t *parser)
{
    hhg_node_t *literal =
        hhg_parser_node_new(parser, parser->lexer->token.type);
    literal->value.literal.str =
        hhg_parser_strdup(parser, parser->lexer->token.str.str);

    hhg_lexer_next(parser->lexer);
    return literal;
}

static hhg_node_t *hhg_parser_parse_arr_literal(hhg_parser_t *parser)
{
    hhg_node_t *arr_literal =
        hhg_parser_node_new(parser, HHG_NODE_ARR_LITERAL);
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
    hhg_node_t *if_stmt = hhg_parser_node_new(parser, HHG_TOKEN_IF);

    if_stmt->value.if_stmt.cond =
        hhg_parser_parse_expr(parser, HHG_PREC_START);
    if_stmt->value.if_stmt.if_body =
        hhg_parser_parse_expr(parser, HHG_PREC_START);

    return if_stmt;
}

static hhg_node_t *hhg_parser_parse_while(hhg_parser_t *parser)
{
    hhg_lexer_next(parser->lexer);
    hhg_node_t *while_stmt = hhg_parser_node_new(parser, HHG_TOKEN_WHILE);

    while_stmt->value.while_stmt.cond =
        hhg_parser_parse_expr(parser, HHG_PREC_START);
    while_stmt->value.while_stmt.body =
        hhg_parser_parse_expr(parser, HHG_PREC_START);

    return while_stmt;
}

static hhg_node_t *hhg_parser_parse_func_decl(hhg_parser_t *parser)
{
    hhg_lexer_next(parser->lexer);
    hhg_node_t *func_decl = hhg_parser_node_new(parser, HHG_TOKEN_DEF);

    if (parser->lexer->token.type == HHG_TOKEN_ID)
        func_decl->value.func_decl.id.str =
            hhg_parser_strdup(parser, parser->lexer->token.str.str);

    hhg_lexer_match(parser->lexer, HHG_TOKEN_ID);
    hhg_lexer_match(parser->lexer, '(');

    while (parser->lexer->token.type != ')' &&
           parser->lexer->token.type != EOF) {
        hhg_node_t *param = hhg_parser_node_new(parser, HHG_NODE_PARAM);

        hhg_file_pos_t start_pos = parser->lexer->token.range.start;

        param->value_type = hhg_parser_parse_type(parser);

        if (parser->lexer->token.type == HHG_TOKEN_ID)
            param->value.param.id.str =
                hhg_parser_strdup(parser, parser->lexer->token.str.str);

        param->range = (hhg_file_range_t) {
            .start = start_pos,
            .end = parser->lexer->token.range.end
        };

        hhg_lexer_match(parser->lexer, HHG_TOKEN_ID);

        arrput(func_decl->value.func_decl.params, param);
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
    hhg_node_t *ret_stmt = hhg_parser_node_new(parser, HHG_TOKEN_RETURN);
    ret_stmt->value.ret.expr =
        hhg_parser_parse_expr(parser, HHG_PREC_START);
    return ret_stmt;
}

static hhg_node_t *hhg_parser_parse_class_decl(hhg_parser_t *parser)
{
    hhg_lexer_next(parser->lexer);
    hhg_node_t *class_decl = hhg_parser_node_new(parser, HHG_TOKEN_CLASS);
    if (parser->lexer->token.type == HHG_TOKEN_ID) {
        class_decl->value.class_decl.id.str =
            hhg_parser_strdup(parser, parser->lexer->token.str.str);

        hhg_type_t *class_type = hhg_type_new(HHG_TYPE_ID, parser->arena);

        class_type->info.id = class_decl->value.class_decl.id.str;

        hhg_sym_tab_insert(
            parser->sym_tab,
            (hhg_sym_t) {
                .key = class_decl->value.class_decl.id.str,
                .value = (hhg_sym_value_t) {
                    .sym_type = HHG_SYM_CLASS,
                    .type = class_type,
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
                hhg_parser_parse_unary(parser) // use parse_unary to get range
            );
        else {
            hhg_node_t *var_decl = hhg_parser_node_new(parser, '=');
            var_decl->range.start = parser->lexer->token.range.start;
            var_decl->value_type = hhg_parser_parse_type(parser);

            if (var_decl->value_type == NULL)
                hhg_parser_error(
                    parser,
                    "expected type in class variable declaration",
                    "here"
                );

            if (parser->lexer->token.type == HHG_TOKEN_ID)
                var_decl->value.var_decl.id.str =
                    hhg_parser_strdup(parser, parser->lexer->token.str.str);

            var_decl->range.end = parser->lexer->token.range.end;

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
    hhg_node_t *block = hhg_parser_node_new(parser, HHG_NODE_BLOCK);

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

static hhg_node_t *hhg_parser_node_new(
    hhg_parser_t *parser,
    hhg_node_type_t type
)
{
    return hhg_node_new(parser->arena, type, &parser->lexer->src);
}

static char *hhg_parser_strdup(hhg_parser_t *parser, const char *str)
{
    return hhg_arena_strdup(parser->arena, str);
}
