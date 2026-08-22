#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <stb_ds.h>

#include "msg.h"
#include "node.h"
#include "sema.h"
#include "sym.h"
#include "sym_tab.h"
#include "type.h"
#include "type_ctx.h"
#include "utils.h"

#define hhg_sema_msg(sema, type, node, ...)                                    \
    hhg_msg(sema->msg_ctx, type, node->src, &node->range, __VA_ARGS__)

#define hhg_sema_error(sema, node, ...)                                        \
    hhg_sema_msg(sema, HHG_MSG_ERROR, node, __VA_ARGS__)

#define hhg_sema_warning(sema, node, ...)                                      \
    hhg_sema_msg(sema, HHG_MSG_WARNING, node, __VA_ARGS__)

#define hhg_sema_info(sema, node, ...)                                         \
    hhg_sema_msg(sema, HHG_MSG_INFO, node, __VA_ARGS__)

typedef struct hhg_int_lit_max_data {
    uint64_t value;
    hhg_base_type_t type;
} hhg_int_lit_max_data_t;

hhg_int_lit_max_data_t hhg_int_lit_max_data[] = {
    {INT8_MAX, HHG_TYPE_I8},   {UINT8_MAX, HHG_TYPE_U8},
    {INT16_MAX, HHG_TYPE_I16}, {UINT16_MAX, HHG_TYPE_U16},
    {INT32_MAX, HHG_TYPE_I32}, {UINT32_MAX, HHG_TYPE_U32},
    {INT64_MAX, HHG_TYPE_I64}, {UINT64_MAX, HHG_TYPE_U64},
};

static void hhg_sema_run_children(hhg_sema_t *sema, hhg_node_t **children);
static void hhg_sema_run_id(hhg_sema_t *sema, hhg_node_t *node);
static void hhg_sema_run_block(hhg_sema_t *sema, hhg_node_t *node);
static void hhg_sema_run_fn_decl(hhg_sema_t *sema, hhg_node_t *node);
static void hhg_sema_run_var_decl(hhg_sema_t *sema, hhg_node_t *node);
static void hhg_sema_run_if(hhg_sema_t *sema, hhg_node_t *node);
static void hhg_sema_run_while(hhg_sema_t *sema, hhg_node_t *node);
static void hhg_sema_run_return(hhg_sema_t *sema, hhg_node_t *node);
static void hhg_sema_run_for(hhg_sema_t *sema, hhg_node_t *node);
static void hhg_sema_run_int_lit(hhg_sema_t *sema, hhg_node_t *node);
static void hhg_sema_run_float_lit(hhg_sema_t *sema, hhg_node_t *node);
static void hhg_sema_run_str_lit(hhg_sema_t *sema, hhg_node_t *node);
static void hhg_sema_run_char_lit(hhg_sema_t *sema, hhg_node_t *node);
static void hhg_sema_run_bool_lit(hhg_sema_t *sema, hhg_node_t *node);
static void hhg_sema_run_arr_lit(hhg_sema_t *sema, hhg_node_t *node);
static void hhg_sema_run_expr(hhg_sema_t *sema, hhg_node_t *node);
static void hhg_sema_run_eq(hhg_sema_t *sema, hhg_node_t *node);
static void hhg_sema_run_range(hhg_sema_t *sema, hhg_node_t *node);
static void hhg_sema_run_neg(hhg_sema_t *sema, hhg_node_t *node);
static void hhg_sema_run_bit_not(hhg_sema_t *sema, hhg_node_t *node);
static void hhg_sema_run_deref(hhg_sema_t *sema, hhg_node_t *node);
static void hhg_sema_run_ref(hhg_sema_t *sema, hhg_node_t *node);
static void hhg_sema_run_arr_idx(hhg_sema_t *sema, hhg_node_t *node);
static void hhg_sema_run_fn_call(hhg_sema_t *sema, hhg_node_t *node);
static int32_t hhg_sema_char_to_int(char c, int32_t base);

void hhg_sema_init(hhg_sema_t *sema, hhg_sym_tab_t *sym_tab,
                   hhg_type_ctx_t *type_ctx, hhg_msg_ctx_t *msg_ctx,
                   hhg_arena_t *arena)
{
    *sema = (hhg_sema_t){
        .sym_tab = sym_tab,
        .type_ctx = type_ctx,
        .msg_ctx = msg_ctx,
        .arena = arena,
    };
}

void hhg_sema_run(hhg_sema_t *sema, hhg_node_t *node)
{
    switch (node->type) {
    case HHG_NODE_ID:
        hhg_sema_run_id(sema, node);
        break;
    case HHG_NODE_BLOCK:
        hhg_sema_run_block(sema, node);
        break;
    case HHG_NODE_FN_DECL:
        hhg_sema_run_fn_decl(sema, node);
        break;
    case HHG_NODE_VAR_DECL:
        hhg_sema_run_var_decl(sema, node);
        break;
    case HHG_NODE_IF:
        hhg_sema_run_if(sema, node);
        break;
    case HHG_NODE_WHILE:
        hhg_sema_run_while(sema, node);
        break;
    case HHG_NODE_RETURN:
        hhg_sema_run_return(sema, node);
        break;
    case HHG_NODE_FOR:
        hhg_sema_run_for(sema, node);
        break;
    case HHG_NODE_INT_LIT:
        hhg_sema_run_int_lit(sema, node);
        break;
    case HHG_NODE_FLOAT_LIT:
        hhg_sema_run_float_lit(sema, node);
        break;
    case HHG_NODE_CHAR_LIT:
        hhg_sema_run_char_lit(sema, node);
        break;
    case HHG_NODE_STR_LIT:
        hhg_sema_run_str_lit(sema, node);
        break;
    case HHG_NODE_BOOL_LIT:
        hhg_sema_run_bool_lit(sema, node);
        break;
    case HHG_NODE_ARR_LIT:
        hhg_sema_run_arr_lit(sema, node);
        break;
    case HHG_NODE_ADD:
    case HHG_NODE_SUB:
    case HHG_NODE_MUL:
    case HHG_NODE_DIV:
    case HHG_NODE_MOD:
    case HHG_NODE_BIT_AND:
    case HHG_NODE_BIT_OR:
    case HHG_NODE_BIT_XOR:
    case HHG_NODE_LSHIFT:
    case HHG_NODE_RSHIFT:
    case HHG_NODE_EQ_EQ:
    case HHG_NODE_NOT_EQ:
    case HHG_NODE_LT:
    case HHG_NODE_LT_EQ:
    case HHG_NODE_GT:
    case HHG_NODE_GT_EQ:
    case HHG_NODE_AND:
    case HHG_NODE_OR:
    case HHG_NODE_NOT:
        hhg_sema_run_expr(sema, node);
        break;
    case HHG_NODE_EQ:
        hhg_sema_run_eq(sema, node);
        break;
    case HHG_NODE_NEG:
        hhg_sema_run_neg(sema, node);
        break;
    case HHG_NODE_RANGE_EXCL:
    case HHG_NODE_RANGE_INCL:
        hhg_sema_run_range(sema, node);
        break;
    case HHG_NODE_BIT_NOT:
        hhg_sema_run_bit_not(sema, node);
        break;
    case HHG_NODE_DEREF:
        hhg_sema_run_deref(sema, node);
        break;
    case HHG_NODE_REF:
        hhg_sema_run_ref(sema, node);
        break;
    case HHG_NODE_ARR_IDX:
        hhg_sema_run_arr_idx(sema, node);
        break;
    case HHG_NODE_FN_CALL:
        hhg_sema_run_fn_call(sema, node);
        break;
    default:
        hhg_compiler_error("unhandled node type `%n` in `%s`", node->type,
                           __func__);
        break;
    }
}

static void hhg_sema_run_children(hhg_sema_t *sema, hhg_node_t **children)
{
    size_t len = arrlenu(children);
    for (size_t i = 0; i < len; i++)
        hhg_sema_run(sema, children[i]);
}

static void hhg_sema_run_id(hhg_sema_t *sema, hhg_node_t *node)
{
    hhg_sym_t *sym = hhg_sym_tab_lookup(sema->sym_tab, node->value.id.str);
    if (sym == NULL) {
        hhg_sema_error(sema, node, "undefined variable `%s`", "`%s` used here",
                       node->value.id.str);
        node->value_type =
            hhg_type_ctx_get_builtin(sema->type_ctx, HHG_TYPE_NONE);
    } else
        node->value_type = sym->value.type;

    node->value.id.sym = sym;
}

static void hhg_sema_run_block(hhg_sema_t *sema, hhg_node_t *node)
{
    hhg_sym_tab_enter_scope(sema->sym_tab);
    hhg_sema_run_children(sema, node->value.block.body);
    hhg_sym_tab_exit_scope(sema->sym_tab);
}

static void hhg_sema_run_var_decl(hhg_sema_t *sema, hhg_node_t *node)
{
    // run sema on expr first to handle `let x = x` where x is not defined yet
    hhg_sema_run(sema, node->value.var_decl.value);

    const char *name = node->value.var_decl.id.str;

    hhg_type_t *value_type = node->value.var_decl.value->value_type;

    // type inference
    if (node->value_type->type == HHG_TYPE_NONE)
        node->value_type = node->value.var_decl.value->value_type;
    else if (!hhg_type_impl_eq(value_type, node->value_type))
        hhg_sema_error(
            sema, node,
            "type mismatch: variable `%s` declared with type `%T` but "
            "expression has type `%T`",
            "here", name, node->value_type, value_type);

    node->value.var_decl.id.sym =
        hhg_sym_tab_insert(sema->sym_tab, (hhg_sym_t){
                                              .key = name,
                                              .value =
                                                  {
                                                      .sym_type = HHG_SYM_VAR,
                                                      .type = node->value_type,
                                                  },
                                          });
}

static void hhg_sema_run_fn_decl(hhg_sema_t *sema, hhg_node_t *node)
{
    hhg_sym_t *sym =
        hhg_sym_tab_lookup(sema->sym_tab, node->value.fn_decl.id.str);
    if (sym != NULL)
        hhg_sema_error(sema, node, "redeclaration of function `%s`",
                       "`%s` declared here", node->value.fn_decl.id.str);

    hhg_type_t *fn_type = hhg_type_new(HHG_TYPE_FN, sema->arena);
    fn_type->value.fn = (hhg_type_fn_t){
        .ret = node->value.fn_decl.ret,
        .params = NULL,
    };

    node->value_type = fn_type;

    hhg_sym_t func_sym = {
        .key = node->value.fn_decl.id.str,
        .value =
            {
                .sym_type = HHG_SYM_FN,
                .type = fn_type,
            },
    };

    node->value.fn_decl.id.sym = hhg_sym_tab_insert(sema->sym_tab, func_sym);

    fn_type->value.fn.sym = node->value.fn_decl.id.sym;

    hhg_sym_tab_enter_scope(sema->sym_tab);

    size_t len = arrlenu(node->value.fn_decl.params);
    for (size_t i = 0; i < len; i++) {
        hhg_node_t *param = node->value.fn_decl.params[i];

        // type linked to sym_tab
        arrput(fn_type->value.fn.params, param->value_type);

        param->value.param.id.sym = hhg_sym_tab_insert(
            sema->sym_tab, (hhg_sym_t){
                               .key = param->value.param.id.str,
                               .value =
                                   {
                                       .sym_type = HHG_SYM_PARAM,
                                       .type = param->value_type,
                                   },
                           });
    }

    hhg_sema_run(sema, node->value.fn_decl.body);
    hhg_sym_tab_exit_scope(sema->sym_tab);
}

static void hhg_sema_run_if(hhg_sema_t *sema, hhg_node_t *node)
{
    hhg_sema_run(sema, node->value.if_stmt.cond);
    hhg_sema_run(sema, node->value.if_stmt.if_body);
}

static void hhg_sema_run_while(hhg_sema_t *sema, hhg_node_t *node)
{
    hhg_sema_run(sema, node->value.while_stmt.cond);
    hhg_sema_run(sema, node->value.while_stmt.body);
}

static void hhg_sema_run_return(hhg_sema_t *sema, hhg_node_t *node)
{
    hhg_sema_run(sema, node->value.return_stmt.value);
    node->value_type = node->value.return_stmt.value->value_type;
}

static void hhg_sema_run_for(hhg_sema_t *sema, hhg_node_t *node)
{
    if (node->value.for_stmt.iter->type != HHG_NODE_RANGE_EXCL &&
        node->value.for_stmt.iter->type != HHG_NODE_RANGE_INCL)
        hhg_todo("for loop iterator must be a range");

    hhg_sema_run_range(sema, node->value.for_stmt.iter);

    hhg_base_type_t iter_type = node->value.for_stmt.iter->value_type->type;

    hhg_sym_t *sym = hhg_sym_tab_insert(
        sema->sym_tab,
        (hhg_sym_t){
            .key = node->value.for_stmt.id.str,
            .value =
                {
                    .sym_type = HHG_SYM_VAR,
                    .type = hhg_type_ctx_get_builtin(sema->type_ctx, iter_type),
                },
        });
    node->value.for_stmt.id.sym = sym;

    hhg_sema_run(sema, node->value.for_stmt.body);
}

static void hhg_sema_run_int_lit(hhg_sema_t *sema, hhg_node_t *node)
{
    uint64_t v = 0;
    const char *p = node->value.int_lit.str;

    int32_t base = 10;

    if (*p == '0') {
        switch (*p++) {
        case 'x':
            base = 16;
            p++;
            break;
        case 'b':
            base = 2;
            p++;
            break;
        case 'o':
            base = 8;
            p++;
            break;
        case '\0':
            node->value_type =
                hhg_type_ctx_get_builtin(sema->type_ctx, HHG_TYPE_U8);
            return;
        default:
            // lexer should have caught this, but just in case
            hhg_compiler_error("unknown base prefix `%c`", *p);
            break;
        }
    }

    while (true) {
        char c = *p;
        if (c == '\0')
            break;

        int32_t digit = hhg_sema_char_to_int(c, base);

        if (v > (UINT64_MAX - digit) / base) {
            hhg_sema_error(
                sema, node,
                // manually format UINT64_MAX to ensure it is in decimal
                "int literal `%s` is too large, max is "
                "`18446744073709551615`",
                "here", node->value.int_lit.str);
            return;
        }
        v *= base;
        v += digit;
        p++;
    }

    for (size_t i = 0; i < HHG_ARR_LEN(hhg_int_lit_max_data); i++) {
        if (v <= hhg_int_lit_max_data[i].value) {
            node->value_type = hhg_type_ctx_get_builtin(
                sema->type_ctx, hhg_int_lit_max_data[i].type);
            return;
        }
    }

    // unreachable
    hhg_compiler_error("int literal type inference failed");
}

static void hhg_sema_run_float_lit(hhg_sema_t *sema, hhg_node_t *node)
{
    const char *point = strchr(node->value.float_lit.str, '.');
    hhg_assert(point != NULL);

    size_t digits = strlen(point + 1);
    if (digits <= 7) {
        node->value_type =
            hhg_type_ctx_get_builtin(sema->type_ctx, HHG_TYPE_F32);
        node->value.float_lit.data.f32 =
            strtof(node->value.float_lit.str, NULL);
    } else {
        node->value_type =
            hhg_type_ctx_get_builtin(sema->type_ctx, HHG_TYPE_F64);
        node->value.float_lit.data.f64 =
            strtod(node->value.float_lit.str, NULL);
        if (digits > 15)
            hhg_sema_warning(sema, node,
                             "float literal `%s` has more than 15 digits; "
                             "after the decimal point, precision may be lost",
                             "here", node->value.float_lit.str);
    }
}

static void hhg_sema_run_str_lit(hhg_sema_t *sema, hhg_node_t *node)
{
    node->value_type = hhg_type_ctx_new_type(
        sema->type_ctx, (hhg_type_t){.type = HHG_TYPE_ARR,
                                     .value.arr = (hhg_type_arr_t){
                                         .elem = hhg_type_ctx_get_builtin(
                                             sema->type_ctx, HHG_TYPE_CHAR),
                                         .size = NULL,
                                     }});
}

static void hhg_sema_run_char_lit(hhg_sema_t *sema, hhg_node_t *node)
{
    node->value_type = hhg_type_ctx_get_builtin(sema->type_ctx, HHG_TYPE_CHAR);
}

static void hhg_sema_run_bool_lit(hhg_sema_t *sema, hhg_node_t *node)
{
    node->value_type = hhg_type_ctx_get_builtin(sema->type_ctx, HHG_TYPE_BOOL);
}

static void hhg_sema_run_arr_lit(hhg_sema_t *sema, hhg_node_t *node)
{
    hhg_sema_run_children(sema, node->value.arr_lit.elems);
    hhg_type_t *elem_type = node->value_type;

    size_t len = arrlenu(node->value.arr_lit.elems);
    for (size_t i = 0; i < len; i++) {
        hhg_node_t *elem = node->value.arr_lit.elems[i];
        if (elem_type == NULL)
            elem_type = elem->value_type;
        else if (!hhg_type_impl_eq(elem_type, elem->value_type))
            hhg_sema_error(sema, elem,
                           "unexpected type in array literal: expected"
                           "`%T` but got `%T`",
                           "here", elem_type, elem->value_type);
    }
    if (node->value_type == NULL)
        node->value_type =
            hhg_type_ctx_new_type(sema->type_ctx, (hhg_type_t){
                                                      .type = HHG_TYPE_ARR,
                                                      .value.arr =
                                                          {
                                                              .elem = elem_type,
                                                              .size = NULL,
                                                          },
                                                  });
}

static void hhg_sema_run_expr(hhg_sema_t *sema, hhg_node_t *node)
{
    hhg_sema_run(sema, node->value.expr.left);
    hhg_sema_run(sema, node->value.expr.right);
}

static void hhg_sema_run_eq(hhg_sema_t *sema, hhg_node_t *node)
{
    hhg_sema_run(sema, node->value.eq.left);
    hhg_sema_run(sema, node->value.eq.right);
}

static void hhg_sema_run_range(hhg_sema_t *sema, hhg_node_t *node)
{
    hhg_sema_run(sema, node->value.expr.left);
    hhg_sema_run(sema, node->value.expr.right);
    if (!hhg_type_impl_eq(node->value.expr.left->value_type,
                          node->value.expr.right->value_type))
        hhg_sema_error(sema, node,
                       "range start and end must have the same type: got "
                       "`%T` and `%T`",
                       "here", node->value.expr.left->value_type,
                       node->value.expr.right->value_type);
    else
        node->value_type = node->value.expr.left->value_type;
}

static void hhg_sema_run_neg(hhg_sema_t *sema, hhg_node_t *node)
{
    hhg_sema_run(sema, node->value.unary.opnd);
    hhg_type_t *opnd_type = node->value.unary.opnd->value_type;
    if (!hhg_base_type_is_arith(opnd_type->type))
        hhg_sema_error(sema, node,
                       "negation operator `-` requires an arithmetic type: "
                       "got `%T`",
                       "here", opnd_type);
    else
        node->value_type = opnd_type;
}

static void hhg_sema_run_bit_not(hhg_sema_t *sema, hhg_node_t *node)
{
    hhg_sema_run(sema, node->value.unary.opnd);
    hhg_type_t *opnd_type = node->value.unary.opnd->value_type;
    if (!hhg_base_type_is_arith(opnd_type->type))
        hhg_sema_error(sema, node,
                       "bitwise NOT operator `~` requires an arithmetic type: "
                       "got `%T`",
                       "here", opnd_type);
    else
        node->value_type = opnd_type;
}

static void hhg_sema_run_deref(hhg_sema_t *sema, hhg_node_t *node)
{
    hhg_sema_run(sema, node->value.unary.opnd);
    hhg_type_t *opnd_type = node->value.unary.opnd->value_type;
    if (opnd_type->type != HHG_TYPE_REF)
        hhg_sema_error(sema, node,
                       "dereference operator `*` requires a reference type: "
                       "got `%T`",
                       "here", opnd_type);
    else
        node->value_type = opnd_type->value.ref.base;
}

static void hhg_sema_run_ref(hhg_sema_t *sema, hhg_node_t *node)
{
    hhg_sema_run(sema, node->value.ref.opnd);
    hhg_type_t *opnd_type = node->value.ref.opnd->value_type;
    node->value_type = hhg_type_ctx_new_type(
        sema->type_ctx, (hhg_type_t){
                            .type = HHG_TYPE_REF,
                            .value.ref =
                                {
                                    .base = opnd_type,
                                    .qual = node->value.ref.qual,
                                },
                        });
}

static void hhg_sema_run_arr_idx(hhg_sema_t *sema, hhg_node_t *node)
{
    hhg_sema_run(sema, node->value.arr_idx.arr);
    hhg_sema_run(sema, node->value.arr_idx.idx);

    hhg_type_t *arr_type = node->value.arr_idx.arr->value_type;
    hhg_type_t *idx_type = node->value.arr_idx.idx->value_type;

    if (arr_type->type != HHG_TYPE_ARR)
        hhg_sema_error(sema, node,
                       "array index operator `[]` requires an array type: "
                       "got `%T`",
                       "here", arr_type);
    if (!hhg_type_impl_eq(
            idx_type, hhg_type_ctx_get_builtin(sema->type_ctx, HHG_TYPE_USIZE)))
        hhg_sema_error(sema, node,
                       "array index operator `[]` requires an unsigned "
                       "integer type: got `%T`",
                       "here", idx_type);
    else
        node->value_type = arr_type->value.arr.elem;
}

static void hhg_sema_run_fn_call(hhg_sema_t *sema, hhg_node_t *node)
{
    const char *name = node->value.fn_call.fn->value.id.str;
    hhg_sym_t *sym = hhg_sym_tab_lookup(sema->sym_tab, name);
    if (sym == NULL)
        hhg_sema_error(sema, node, "undefined function `%s`",
                       "`%s` called here", name);
    else if (sym->value.sym_type != HHG_SYM_FN)
        hhg_sema_error(sema, node, "`%s` is not a function", "`%s` called here",
                       name);
    else
        node->value_type = sym->value.type->value.fn.ret;

    node->value.fn_call.fn->value.id.sym = sym;

    hhg_sema_run_children(sema, node->value.fn_call.args);
}

static int32_t hhg_sema_char_to_int(char c, int32_t base)
{
    if (base == 'd') {
        if (c >= '0' && c <= '9')
            return c - '0';
    } else if (base == 'x') {
        if (c >= '0' && c <= '9')
            return c - '0';
        else if (c >= 'a' && c <= 'f')
            return c - 'a' + 10;
        else if (c >= 'A' && c <= 'F')
            return c - 'A' + 10;
    } else if (base == 'b') {
        if (c == '0' || c == '1')
            return c - '0';
    } else if (base == 'o') {
        if (c >= '0' && c <= '7')
            return c - '0';
    }
    if (base == 'd')
        hhg_compiler_error("invalid character `%c` in int literal", c);
    else
        hhg_compiler_error("invalid character `%c` for base `%c` int literal",
                           c, base);
    return 0;
}
