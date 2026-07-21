#include <stdint.h>
#include <stdbool.h>

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

#define hhg_parser_msg(parser, type, ...) \
    do {                                  \
        hhg_msg(                          \
            parser->msg_ctx,              \
            type,                         \
            &parser->lexer->src,          \
            &parser->lexer->token.range,  \
            __VA_ARGS__                   \
        );                                \
        hhg_lexer_resync(parser->lexer);  \
    } while (0)

#define hhg_parser_error(parser, ...) \
    hhg_parser_msg(parser, HHG_MSG_ERROR, __VA_ARGS__)

#define hhg_parser_warning(parser, range, ...) \
    hhg_parser_msg(parser, HHG_MSG_WARNING, __VA_ARGS__)

#define hhg_parser_info(parser, range, ...) \
    hhg_parser_msg(parser, HHG_MSG_INFO, range, __VA_ARGS__)

typedef struct hhg_bind_data {
    int32_t prec;
    bool assoc;
} hhg_bind_data_t;

static hhg_node_t *hhg_parser_node_new(
    hhg_parser_t *parser,
    hhg_node_type_t type
);
static char *hhg_parser_strdup(hhg_parser_t *parser, const char *str);

static hhg_node_t *hhg_parser_parse_decl(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_decl_core(hhg_parser_t *parser);

static hhg_node_t *hhg_parser_parse_stmt(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_stmt_core(hhg_parser_t *parser);

static hhg_node_t *hhg_parser_parse_expr(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_expr_core(
    hhg_parser_t *parser,
    int32_t min_prec
);

static hhg_node_t *hhg_parser_parse_unary(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_unary_core(hhg_parser_t *parser);

static hhg_node_t *hhg_parser_parse_core(
    hhg_parser_t *parser,
    hhg_node_t *(*parse_fn)(hhg_parser_t *)
);

static hhg_type_t *hhg_parser_parse_type(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_id(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_block(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_fn_decl(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_var_decl(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_if(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_while(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_return(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_for(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_neg(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_bit_not(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_deref(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_ref(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_int_lit(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_float_lit(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_char_lit(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_str_lit(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_bool_lit(hhg_parser_t *parser);
static hhg_node_t *hhg_parser_parse_arr_lit(hhg_parser_t *parser);

static const hhg_bind_data_t bind_data[HHG_TOKEN_TYPE_COUNT] = {
    [HHG_TOKEN_STAR]       = { 11, true,  },
    [HHG_TOKEN_SLASH]      = { 11, true,  },
    [HHG_TOKEN_PERCENT]    = { 11, true,  },
    [HHG_TOKEN_PLUS]       = { 10, true,  },
    [HHG_TOKEN_MINUS]      = { 10, true,  },
    [HHG_TOKEN_LSHIFT]     = { 9,  true,  },
    [HHG_TOKEN_RSHIFT]     = { 9,  true,  },
    [HHG_TOKEN_AMPERSAND]  = { 8,  true,  },
    [HHG_TOKEN_CARET]      = { 7,  true,  },
    [HHG_TOKEN_PIPE]       = { 6,  true,  },
    [HHG_TOKEN_DOT_DOT]    = { 5,  false, },
    [HHG_TOKEN_DOT_DOT_EQ] = { 5,  false, },
    [HHG_TOKEN_EQ_EQ]      = { 4,  false, },
    [HHG_TOKEN_NOT_EQ]     = { 4,  false, },
    [HHG_TOKEN_LT]         = { 4,  false, },
    [HHG_TOKEN_LT_EQ]      = { 4,  false, },
    [HHG_TOKEN_GT]         = { 4,  false, },
    [HHG_TOKEN_GT_EQ]      = { 4,  false, },
    [HHG_TOKEN_AND]        = { 2,  true,  },
    [HHG_TOKEN_OR]         = { 1,  true,  },
};

void hhg_parser_init(
    hhg_parser_t *parser,
    hhg_lexer_t *lexer,
    hhg_type_ctx_t *type_ctx,
    hhg_msg_ctx_t *msg_ctx,
    hhg_arena_t *arena
)
{
    *parser = (hhg_parser_t) {
        .lexer = lexer,
        .type_ctx = type_ctx,
        .msg_ctx = msg_ctx,
        .arena = arena,
        .stmt = false
    };
}

hhg_node_t *hhg_parser_parse(hhg_parser_t *parser)
{
    hhg_node_t *prog = hhg_parser_node_new(parser, HHG_NODE_BLOCK);

    hhg_lexer_next(parser->lexer);

    if (parser->lexer->token.type == HHG_TOKEN_EOF) {
        prog->range = parser->lexer->token.range;
        return prog;
    }

    hhg_file_pos_t start_pos = parser->lexer->token.range.start;

    do {
        arrput(
            prog->value.block.body,
            hhg_parser_parse_decl(parser)
        );
        if (parser->lexer->token.type == HHG_TOKEN_EOF)
            break;
        
        hhg_lexer_match(parser->lexer, HHG_TOKEN_NEWLINE);
    } while (parser->lexer->token.type != HHG_TOKEN_EOF);

    prog->range = (hhg_file_range_t) {
        .start = start_pos,
        .end = parser->lexer->token.range.end
    };
    return prog;
}

void hhg_parser_del(hhg_parser_t *parser)
{
    HHG_UNUSED(parser);
    // nothing to do for now...
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

static hhg_node_t *hhg_parser_parse_decl(hhg_parser_t *parser)
{
    return hhg_parser_parse_core(parser, hhg_parser_parse_decl_core);
}

static hhg_node_t *hhg_parser_parse_decl_core(hhg_parser_t *parser)
{
    switch (parser->lexer->token.type) {
    case HHG_TOKEN_FN:
        return hhg_parser_parse_fn_decl(parser);
    case HHG_TOKEN_LET:
    case HHG_TOKEN_CONST:
        return hhg_parser_parse_var_decl(parser);
    default:
        hhg_parser_error(
            parser,
            "expected declaration, got `%t`",
            "here",
            parser->lexer->token.type
        );
        return hhg_parser_node_new(parser, HHG_NODE_NONE);
    }
}

static hhg_node_t *hhg_parser_parse_stmt(hhg_parser_t *parser)
{
    return hhg_parser_parse_core(parser, hhg_parser_parse_stmt_core);
}

static hhg_node_t *hhg_parser_parse_stmt_core(hhg_parser_t *parser)
{
    switch (parser->lexer->token.type) {
    case HHG_TOKEN_IF:
        return hhg_parser_parse_if(parser);
    case HHG_TOKEN_WHILE:
        return hhg_parser_parse_while(parser);
    case HHG_TOKEN_RETURN:
        return hhg_parser_parse_return(parser);
    case HHG_TOKEN_FOR:
        return hhg_parser_parse_for(parser);
    case HHG_TOKEN_LBRACE:
        return hhg_parser_parse_block(parser);
    case HHG_TOKEN_LET:
    case HHG_TOKEN_CONST:
        return hhg_parser_parse_var_decl(parser);
    default:
        parser->stmt = true;
        hhg_node_t *expr = hhg_parser_parse_expr(parser);
        parser->stmt = false;
        return expr;
    }
}

static hhg_node_t *hhg_parser_parse_expr(hhg_parser_t *parser)
{
    return hhg_parser_parse_expr_core(parser, 0);
}

static hhg_node_t *hhg_parser_parse_unary(hhg_parser_t *parser)
{
    hhg_node_t *node =
        hhg_parser_parse_core(parser, hhg_parser_parse_unary_core);

    switch (parser->lexer->token.type) {
    case HHG_TOKEN_LBRACKET: {
        hhg_node_t *arr_idx =
            hhg_parser_node_new(parser, HHG_NODE_ARR_IDX);

        arr_idx->value.expr.left = node;
        hhg_lexer_next(parser->lexer);

        arr_idx->value.expr.right = hhg_parser_parse_expr(parser);
        hhg_lexer_match(parser->lexer, HHG_TOKEN_RBRACKET);

        arr_idx->range = (hhg_file_range_t) {
            .start = node->range.start,
            .end = parser->lexer->last_pos
        };
        return arr_idx;
    }
    case HHG_TOKEN_LPAREN: {
        hhg_node_t *fn_call =
            hhg_parser_node_new(parser, HHG_NODE_FN_CALL);
        fn_call->value.fn_call.fn = node;

        hhg_lexer_next(parser->lexer);
        while (parser->lexer->token.type != HHG_TOKEN_RPAREN &&
               parser->lexer->token.type != HHG_TOKEN_EOF) {
            arrput(
                fn_call->value.fn_call.args,
                hhg_parser_parse_expr(parser)
            );
            if (parser->lexer->token.type != HHG_TOKEN_RPAREN &&
                parser->lexer->token.type != HHG_TOKEN_EOF)
                hhg_lexer_match(parser->lexer, HHG_TOKEN_COMMA);
        }
        hhg_lexer_match(parser->lexer, HHG_TOKEN_RPAREN);
        return fn_call;
    }
    default:
        return node;
    }
}
static hhg_node_t *hhg_parser_parse_unary_core(hhg_parser_t *parser) {
    switch (parser->lexer->token.type) {
    case HHG_TOKEN_ID:
        return hhg_parser_parse_id(parser);
    case HHG_TOKEN_LPAREN: {
        hhg_lexer_next(parser->lexer);
        hhg_node_t *expr = hhg_parser_parse_expr(parser);
        hhg_lexer_match(parser->lexer, HHG_TOKEN_RPAREN);
        return expr;
    }
    case HHG_TOKEN_INT_LIT:
        return hhg_parser_parse_int_lit(parser);
    case HHG_TOKEN_FLOAT_LIT:
        return  hhg_parser_parse_float_lit(parser);
    case HHG_TOKEN_CHAR_LIT:
        return hhg_parser_parse_char_lit(parser);
    case HHG_TOKEN_STR_LIT:
        return hhg_parser_parse_str_lit(parser);
    case HHG_TOKEN_TRUE:
    case HHG_TOKEN_FALSE:
        return hhg_parser_parse_bool_lit(parser);
    case HHG_TOKEN_MINUS:
        return hhg_parser_parse_neg(parser);
    case HHG_TOKEN_TILDE:
        return hhg_parser_parse_bit_not(parser);
    case HHG_TOKEN_STAR:
        return hhg_parser_parse_deref(parser);
    case HHG_TOKEN_AMPERSAND:
        return hhg_parser_parse_ref(parser);
    case HHG_TOKEN_LBRACKET:
        return hhg_parser_parse_arr_lit(parser);
    default:
        hhg_parser_error(
            parser,
            "expected expression, got `%t`",
            "here",
            parser->lexer->token.type
        );
        return NULL;
    }
}

/*
Pratt Parser implementation
hard to understand from code,
see https://martin.janiczek.cz/2023/07/03/demystifying-pratt-parsers.html
to step through the algorithm
*/
static hhg_node_t *hhg_parser_parse_expr_core(
    hhg_parser_t *parser,
    int32_t min_prec
)
{
    hhg_node_t *left = hhg_parser_parse_unary(parser);
    while (true) {
        hhg_bind_data_t bind = bind_data[parser->lexer->token.type];
        if (bind.prec == 0)
            break;
        if (bind.prec < min_prec)
            break;
        hhg_node_t *new_left =
            hhg_parser_node_new(
                parser,
                hhg_token_type_to_node_type(parser->lexer->token.type)
            );
        new_left->value.expr.left = left;

        hhg_lexer_next(parser->lexer);

        new_left->value.expr.right =
            hhg_parser_parse_expr_core(parser, bind.prec);

        new_left->range = (hhg_file_range_t) {
            .start = left->range.start,
            .end = new_left->value.expr.right->range.end
        };

        left = new_left;
        if (!bind.assoc) {
            hhg_bind_data_t next = bind_data[parser->lexer->token.type];
            if (next.prec == bind.prec && !next.assoc) {
                hhg_parser_error(
                    parser,
                    "operator `%t` cannot be chained, add parentheses",
                    "`%t` repeated here",
                    parser->lexer->token.type
                );
                break;
            }
        }
    }

    return left;
}

static hhg_node_t *hhg_parser_parse_core(
    hhg_parser_t *parser,
    hhg_node_t *(*parse_fn)(hhg_parser_t *)
)
{
    hhg_file_pos_t start_pos = parser->lexer->token.range.start;
    hhg_node_t *node = parse_fn(parser);
    node->range = (hhg_file_range_t) {
        .start = start_pos,
        .end = parser->lexer->last_pos
    };
    return node;
}


static hhg_type_t *hhg_parser_parse_type(hhg_parser_t *parser)
{
    hhg_base_type_t base_type =
        hhg_token_type_to_base_type(parser->lexer->token.type);
    switch (base_type) {
    case HHG_TYPE_I8:
    case HHG_TYPE_U8:
    case HHG_TYPE_I16:
    case HHG_TYPE_U16:
    case HHG_TYPE_I32:
    case HHG_TYPE_U32:
    case HHG_TYPE_I64:
    case HHG_TYPE_U64:
    case HHG_TYPE_F32:
    case HHG_TYPE_F64:
    case HHG_TYPE_BOOL:
    case HHG_TYPE_CHAR:
    case HHG_TYPE_ISIZE:
    case HHG_TYPE_USIZE:
    case HHG_TYPE_VOID: {
        hhg_type_t *type =
            hhg_type_ctx_get_builtin(parser->type_ctx, base_type);
        hhg_lexer_next(parser->lexer);
        while (parser->lexer->token.type == HHG_TOKEN_LBRACKET) {
            hhg_lexer_next(parser->lexer);

            hhg_node_t *size = hhg_parser_parse_expr(parser);
            if (size->type != HHG_NODE_INT_LIT)
                hhg_todo("array size must be an integer literal");

            type = hhg_type_ctx_new_type(
                parser->type_ctx,
                (hhg_type_t) {
                    .type = HHG_TYPE_ARR,
                    .value.arr = (hhg_type_arr_t) {
                        .elem = type,
                        .size = size,
                    },
                }
            );
            
            hhg_lexer_match(parser->lexer, HHG_TOKEN_RBRACKET);
        }
        return type;
    }
    case HHG_TYPE_REF: {
        hhg_lexer_next(parser->lexer);
        hhg_ref_qual_t qual = HHG_REF_QUAL_IMMUT;
        if (parser->lexer->token.type == HHG_TOKEN_MUT) {
            hhg_lexer_next(parser->lexer);
            qual = HHG_REF_QUAL_MUT;
        }
        hhg_type_t *base = hhg_parser_parse_type(parser);
        return hhg_type_ctx_new_type(
            parser->type_ctx,
            (hhg_type_t) {
                .type = HHG_TYPE_REF,
                .value.ref = (hhg_type_ref_t) {
                    .qual = qual,
                    .base = base,
                },
            }
        ); 
    }
    case HHG_TYPE_NONE: {
        if (parser->lexer->token.type == HHG_TOKEN_ID)
            hhg_todo("class types are currently unsupported");
        hhg_parser_error(
            parser,
            "expected type, got `%t`",
            "here",
            parser->lexer->token.type
        );
        return NULL;
    }
    default:
        hhg_fatal_error(
            "unexpected base type `%d` in `hhg_parser_parse_type`",
            base_type
        );
        return NULL;
    }
}

static hhg_node_t *hhg_parser_parse_id(hhg_parser_t *parser)
{
    const char *str = hhg_parser_strdup(parser, parser->lexer->token.str.str);
    
    hhg_lexer_next(parser->lexer);
    switch (parser->lexer->token.type) {
    case HHG_TOKEN_EQ:
    case HHG_TOKEN_PLUS_EQ:
    case HHG_TOKEN_MINUS_EQ:
    case HHG_TOKEN_STAR_EQ:
    case HHG_TOKEN_SLASH_EQ:
    case HHG_TOKEN_PERCENT_EQ:
    case HHG_TOKEN_AMPERSAND_EQ:
    case HHG_TOKEN_PIPE_EQ:
    case HHG_TOKEN_CARET_EQ:
    case HHG_TOKEN_LSHIFT_EQ:
    case HHG_TOKEN_RSHIFT_EQ: {
        if (!parser->stmt)
            hhg_parser_error(
                parser,
                "assignment is not an expression",
                "here"
            );
        hhg_node_t *assign = hhg_parser_node_new(parser, HHG_NODE_EQ);

        assign->value.eq.left = hhg_parser_node_new(parser, HHG_NODE_ID);
        assign->value.eq.left->value.id.str = str;

        hhg_lexer_next(parser->lexer);
        assign->value.eq.right = hhg_parser_parse_expr(parser);

        return assign;
    }
    default: {
        hhg_node_t *id = hhg_parser_node_new(parser, HHG_NODE_ID);
        id->value.id.str = str;
        return id;
    }
    }
}

static hhg_node_t *hhg_parser_parse_block(hhg_parser_t *parser)
{
    hhg_node_t *block = hhg_parser_node_new(parser, HHG_NODE_BLOCK);
    hhg_lexer_match(parser->lexer, HHG_TOKEN_LBRACE);

    while (parser->lexer->token.type != HHG_TOKEN_RBRACE &&
           parser->lexer->token.type != HHG_TOKEN_EOF) {
        arrput(
            block->value.block.body,
            hhg_parser_parse_stmt(parser)
        );
        
        hhg_lexer_match(parser->lexer, HHG_TOKEN_NEWLINE);
    }

    hhg_lexer_match(parser->lexer, HHG_TOKEN_RBRACE);
    return block;
}

static hhg_node_t *hhg_parser_parse_fn_decl(hhg_parser_t *parser)
{
    hhg_node_t *fn_decl = hhg_parser_node_new(parser, HHG_NODE_FN_DECL);
    hhg_lexer_next(parser->lexer);
    
    fn_decl->value.fn_decl.id.str =
        hhg_parser_strdup(parser, parser->lexer->token.str.str);
    
    hhg_lexer_match(parser->lexer, HHG_TOKEN_ID);
    hhg_lexer_match(parser->lexer, HHG_TOKEN_LPAREN);

    while (parser->lexer->token.type != HHG_TOKEN_RPAREN &&
           parser->lexer->token.type != HHG_TOKEN_EOF) {
        hhg_node_t *param = hhg_parser_node_new(parser, HHG_NODE_PARAM);
        param->value.param.id.str =
            hhg_parser_strdup(parser, parser->lexer->token.str.str);
        hhg_lexer_match(parser->lexer, HHG_TOKEN_ID);
        hhg_lexer_match(parser->lexer, HHG_TOKEN_COLON);
        param->value_type = hhg_parser_parse_type(parser);
        arrput(
            fn_decl->value.fn_decl.params,
            param
        );
        if (parser->lexer->token.type != HHG_TOKEN_RPAREN &&
            parser->lexer->token.type != HHG_TOKEN_EOF)
            hhg_lexer_match(parser->lexer, HHG_TOKEN_COMMA);
    }

    hhg_lexer_match(parser->lexer, HHG_TOKEN_RPAREN);
    if (parser->lexer->token.type == HHG_TOKEN_ARROW) {
        hhg_lexer_next(parser->lexer);
        fn_decl->value.fn_decl.ret = hhg_parser_parse_type(parser);
    } else
        fn_decl->value.fn_decl.ret =
            hhg_type_ctx_get_builtin(parser->type_ctx, HHG_TYPE_VOID);
    
    fn_decl->value.fn_decl.body = hhg_parser_parse_block(parser);

    return fn_decl;
}

static hhg_node_t *hhg_parser_parse_var_decl(hhg_parser_t *parser)
{
    hhg_qual_t qual = HHG_QUAL_NONE;

    hhg_token_type_t decl_type = parser->lexer->token.type;
    hhg_lexer_next(parser->lexer);

    if (decl_type == HHG_TOKEN_CONST) {
        qual = HHG_QUAL_CONST;
    } else if (parser->lexer->token.type == HHG_TOKEN_MUT) {
        qual = HHG_QUAL_MUT;
        hhg_lexer_next(parser->lexer);
    }

    hhg_node_t *var_decl = hhg_parser_node_new(parser, HHG_NODE_VAR_DECL);
    
    var_decl->value.var_decl.id.str =
        hhg_parser_strdup(parser, parser->lexer->token.str.str);
    var_decl->value.var_decl.qual = qual;

    hhg_lexer_match(parser->lexer, HHG_TOKEN_ID);
    if (parser->lexer->token.type == HHG_TOKEN_COLON) {
        hhg_lexer_next(parser->lexer);
        var_decl->value_type = hhg_parser_parse_type(parser);
    }

    hhg_lexer_match(parser->lexer, HHG_TOKEN_EQ);

    var_decl->value.var_decl.value = hhg_parser_parse_expr(parser);
    return var_decl;
}

static hhg_node_t *hhg_parser_parse_if(hhg_parser_t *parser)
{
    hhg_lexer_next(parser->lexer);
    hhg_node_t *if_stmt = hhg_parser_node_new(parser, HHG_NODE_IF);

    if_stmt->value.if_stmt.cond = hhg_parser_parse_expr(parser);
    if_stmt->value.if_stmt.if_body = hhg_parser_parse_block(parser);

    if (parser->lexer->token.type == HHG_TOKEN_ELSE) {
        hhg_lexer_next(parser->lexer);
        if (parser->lexer->token.type == HHG_TOKEN_IF) {
            if_stmt->value.if_stmt.else_body = hhg_parser_parse_if(parser);
        } else
            if_stmt->value.if_stmt.else_body = hhg_parser_parse_block(parser);
    }

    return if_stmt;
}

static hhg_node_t *hhg_parser_parse_while(hhg_parser_t *parser)
{
    hhg_lexer_next(parser->lexer);
    hhg_node_t *while_stmt = hhg_parser_node_new(parser, HHG_NODE_WHILE);

    while_stmt->value.while_stmt.cond = hhg_parser_parse_expr(parser);
    while_stmt->value.while_stmt.body = hhg_parser_parse_stmt(parser);

    return while_stmt;
}

static hhg_node_t *hhg_parser_parse_return(hhg_parser_t *parser)
{
    hhg_lexer_next(parser->lexer);
    hhg_node_t *return_stmt = hhg_parser_node_new(parser, HHG_NODE_RETURN);

    return_stmt->value.return_stmt.value = hhg_parser_parse_expr(parser);

    return return_stmt;
}

static hhg_node_t *hhg_parser_parse_for(hhg_parser_t *parser)
{
    hhg_lexer_next(parser->lexer);
    hhg_node_t *for_stmt = hhg_parser_node_new(parser, HHG_NODE_FOR);
    for_stmt->value.for_stmt.id.str =
        hhg_parser_strdup(parser, parser->lexer->token.str.str);
    hhg_lexer_match(parser->lexer, HHG_TOKEN_ID);
    hhg_lexer_match(parser->lexer, HHG_TOKEN_IN);


    for_stmt->value.for_stmt.iter = hhg_parser_parse_expr(parser);
    if (for_stmt->value.for_stmt.iter->type != HHG_NODE_RANGE_EXCL &&
        for_stmt->value.for_stmt.iter->type != HHG_NODE_RANGE_INCL)
        hhg_todo("for loop iterator must be a range for now");

    for_stmt->value.for_stmt.body = hhg_parser_parse_stmt(parser);

    return for_stmt;
}

static hhg_node_t *hhg_parser_parse_neg(hhg_parser_t *parser)
{
    hhg_lexer_next(parser->lexer);
    hhg_node_t *neg = hhg_parser_node_new(parser, HHG_NODE_NEG);
    
    neg->value.unary.opnd = hhg_parser_parse_expr(parser);

    return neg;
}

static hhg_node_t *hhg_parser_parse_bit_not(hhg_parser_t *parser)
{
    hhg_lexer_next(parser->lexer);
    hhg_node_t *bit_not = hhg_parser_node_new(parser, HHG_NODE_BIT_NOT);
    
    bit_not->value.unary.opnd = hhg_parser_parse_expr(parser);

    return bit_not;
}

static hhg_node_t *hhg_parser_parse_deref(hhg_parser_t *parser)
{
    hhg_lexer_next(parser->lexer);
    hhg_node_t *deref = hhg_parser_node_new(parser, HHG_NODE_DEREF);

    deref->value.unary.opnd = hhg_parser_parse_expr(parser);

    return deref;
}

static hhg_node_t *hhg_parser_parse_ref(hhg_parser_t *parser)
{
    hhg_lexer_next(parser->lexer);
    hhg_node_t *ref = hhg_parser_node_new(parser, HHG_NODE_REF);
    if (parser->lexer->token.type == HHG_TOKEN_MUT) {
        hhg_lexer_next(parser->lexer);
        ref->value.ref.qual = HHG_REF_QUAL_MUT;
    } else
        ref->value.ref.qual = HHG_REF_QUAL_IMMUT;
    
    ref->value.ref.opnd = hhg_parser_parse_expr(parser);
    return ref;
}

static hhg_node_t *hhg_parser_parse_int_lit(hhg_parser_t *parser)
{
    hhg_node_t *int_lit = hhg_parser_node_new(parser, HHG_NODE_INT_LIT);
    int_lit->value.int_lit.str =
        hhg_parser_strdup(parser, parser->lexer->token.str.str);
    hhg_lexer_next(parser->lexer);
    return int_lit;
}

static hhg_node_t *hhg_parser_parse_float_lit(hhg_parser_t *parser)
{
    hhg_node_t *float_lit = hhg_parser_node_new(parser, HHG_NODE_FLOAT_LIT);
    float_lit->value.float_lit.str =
        hhg_parser_strdup(parser, parser->lexer->token.str.str);
    hhg_lexer_next(parser->lexer);
    return float_lit;
}

static hhg_node_t *hhg_parser_parse_char_lit(hhg_parser_t *parser)
{
    hhg_node_t *char_lit = hhg_parser_node_new(parser, HHG_NODE_CHAR_LIT);
    char_lit->value.char_lit.str =
        hhg_parser_strdup(parser, parser->lexer->token.str.str);
    hhg_lexer_next(parser->lexer);
    return char_lit;
}

static hhg_node_t *hhg_parser_parse_str_lit(hhg_parser_t *parser)
{
    hhg_node_t *str_lit = hhg_parser_node_new(parser, HHG_NODE_STR_LIT);
    str_lit->value.str_lit.str =
        hhg_parser_strdup(parser, parser->lexer->token.str.str);
    hhg_lexer_next(parser->lexer);
    return str_lit;
}

static hhg_node_t *hhg_parser_parse_bool_lit(hhg_parser_t *parser)
{
    hhg_node_t *bool_lit = hhg_parser_node_new(parser, HHG_NODE_BOOL_LIT);
    bool_lit->value.bool_lit.value =
        parser->lexer->token.type == HHG_TOKEN_TRUE;
    hhg_lexer_next(parser->lexer);
    return bool_lit;
}

static hhg_node_t *hhg_parser_parse_arr_lit(hhg_parser_t *parser)
{
    hhg_node_t *arr_lit =
        hhg_parser_node_new(parser, HHG_NODE_ARR_LIT);
    hhg_lexer_next(parser->lexer);
    while (parser->lexer->token.type != HHG_TOKEN_RBRACKET &&
           parser->lexer->token.type != HHG_TOKEN_EOF) {
        arrput(
            arr_lit->value.arr_lit.elems,
            hhg_parser_parse_expr(parser)
        );
        if (parser->lexer->token.type != HHG_TOKEN_RBRACKET &&
            parser->lexer->token.type != HHG_TOKEN_EOF)
            hhg_lexer_match(parser->lexer, HHG_TOKEN_COMMA);
    }
    hhg_lexer_match(parser->lexer, HHG_TOKEN_RBRACKET);
    return arr_lit;
}
