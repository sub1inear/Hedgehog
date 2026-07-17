#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include <stb_ds.h>

#include "node.h"
#include "token.h"
#include "mem.h"
#include "msg.h"
#include "utils.h"

#define HHG_NODE_INDENT_INC 4

const char *const node_type_to_str[] = {
    [HHG_NODE_ID] = "id",
    [HHG_NODE_FN_DECL] = "fn",
    [HHG_NODE_PARAM] = "param",
    [HHG_NODE_VAR_DECL] = "let",
    [HHG_NODE_BLOCK] = "block",
    [HHG_NODE_IF] = "if",
    [HHG_NODE_WHILE] = "while",
    [HHG_NODE_RETURN] = "return",
    [HHG_NODE_FOR] = "for",
    [HHG_NODE_INT_LIT] = "int lit",
    [HHG_NODE_FLOAT_LIT] = "float lit",
    [HHG_NODE_STR_LIT] = "str lit",
    [HHG_NODE_BOOL_LIT] = "bool lit",
    [HHG_NODE_CHAR_LIT] = "char lit",
    [HHG_NODE_ARR_LIT] = "arr lit",
    [HHG_NODE_ADD] = "+",
    [HHG_NODE_SUB] = "-",
    [HHG_NODE_MUL] = "*",
    [HHG_NODE_DIV] = "/",
    [HHG_NODE_MOD] = "%",
    [HHG_NODE_BIT_AND] = "&",
    [HHG_NODE_BIT_OR] = "|",
    [HHG_NODE_BIT_XOR] = "^",
    [HHG_NODE_LSHIFT] = "<<",
    [HHG_NODE_RSHIFT] = ">>",
    [HHG_NODE_ADD_EQ] = "+=",
    [HHG_NODE_SUB_EQ] = "-=",
    [HHG_NODE_MUL_EQ] = "*=",
    [HHG_NODE_DIV_EQ] = "/=",
    [HHG_NODE_MOD_EQ] = "%=",
    [HHG_NODE_BIT_AND_EQ] = "&=",
    [HHG_NODE_BIT_OR_EQ] = "|=",
    [HHG_NODE_BIT_XOR_EQ] = "^=",
    [HHG_NODE_LSHIFT_EQ] = "<<=",
    [HHG_NODE_RSHIFT_EQ] = ">>=",
    [HHG_NODE_EQ_EQ] = "==",
    [HHG_NODE_NOT_EQ] = "!=",
    [HHG_NODE_LT] = "<",
    [HHG_NODE_LT_EQ] = "<=",
    [HHG_NODE_GT] = ">",
    [HHG_NODE_GT_EQ] = ">=",
    [HHG_NODE_AND] = "and",
    [HHG_NODE_OR] = "or",
    [HHG_NODE_NOT] = "not",
    [HHG_NODE_NEG] = "-",
    [HHG_NODE_BIT_NOT] = "~",
    [HHG_NODE_REF] = "ref",
    [HHG_NODE_DEREF] = "deref",
    [HHG_NODE_ARR_IDX] = "arr idx",
    [HHG_NODE_FN_CALL] = "fn call",
    [HHG_NODE_ASSIGN] = "=",
    [HHG_NODE_RANGE] = "range",
};

static void hhg_node_fprint_core(
    hhg_node_t *node,
    int32_t indent,
    hhg_node_print_mode_t mode,
    FILE *stream
);
static void hhg_node_fprint_indent(int32_t indent, FILE *stream);
static void hhg_node_fprint_str(const char *str, int32_t indent, FILE *stream);
static void hhg_node_fprint_id(
    hhg_node_id_t id,
    int32_t indent,
    hhg_node_print_mode_t mode,
    FILE *stream
);

void hhg_node_type_print(hhg_node_type_t type)
{
    hhg_node_type_fprint(type, stdout);
}

void hhg_node_type_fprint(hhg_node_type_t type, FILE *stream)
{
    fputs(hhg_node_type_to_str(type), stream);
}

const char *hhg_node_type_to_str(hhg_node_type_t type)
{
    return node_type_to_str[type];
}

hhg_node_t *hhg_node_new(
    hhg_arena_t *arena,
    hhg_node_type_t type,
    hhg_file_src_t *src
)
{
    hhg_node_t *node = hhg_arena_malloc(arena, sizeof(hhg_node_t));

    // initialize type and src, setting other fields to NULL portably
    *node = (hhg_node_t) { .type = type, .src = src };

    return node;
}

void hhg_node_print(hhg_node_t *node, hhg_node_print_mode_t mode)
{
    hhg_node_fprint(node, mode, stdout);
}

void hhg_node_fprint(
    hhg_node_t *node,
    hhg_node_print_mode_t mode,
    FILE *stream
)
{
    hhg_node_fprint_core(node, 0, mode, stream);
}

void hhg_node_free(hhg_node_t *node)
{
    hhg_type_del(node->value_type);
}

static void hhg_node_fprint_core(
    hhg_node_t *node,
    int32_t indent,
    hhg_node_print_mode_t mode,
    FILE *stream
)
{   
    if (node == NULL)
        return;

    hhg_node_fprint_indent(indent, stream);
    hhg_node_type_fprint(node->type, stream);
    
    if (node->value_type != NULL) {
        fputs(": ", stream);
        hhg_type_fprint(node->value_type, stream);
    }

    fputc('\n', stream);

    int32_t next_indent = indent + HHG_NODE_INDENT_INC;

    switch (node->type) {
    case HHG_NODE_ID:
        hhg_node_fprint_id(node->value.id, next_indent, mode, stream);
        fputc('\n', stream);
        break;
    case HHG_NODE_BLOCK: {
        size_t len = arrlenu(node->value.program.body);
        for (size_t i = 0; i < len; i++)
            hhg_node_fprint_core(node->value.program.body[i], next_indent, mode, stream);
        break;
    }
    case HHG_NODE_FN_DECL:
        hhg_node_fprint_indent(next_indent, stream);
        fputs("fn ", stream);
        hhg_node_fprint_id(node->value.fn_decl.id, 0, mode, stream);
        
        fputc('(', stream);

        size_t len = arrlenu(node->value.fn_decl.params);
        for (size_t i = 0; i < len; i++) {
            hhg_node_fprint_core(node->value.fn_decl.params[i], 0, mode, stream);
            if (i < len - 1)
                fputs(", ", stream);
        }
        
        fputs(") -> ", stream);
        hhg_type_fprint(node->value.fn_decl.return_type, stream);
        
        fputc('\n', stream);

        hhg_node_fprint_core(node->value.fn_decl.body, next_indent, mode, stream);
        break;
    case HHG_NODE_PARAM:
        hhg_node_fprint_id(node->value.param.id, next_indent, mode, stream);
        fputc('\n', stream);
        break;
    case HHG_NODE_VAR_DECL:
        hhg_node_fprint_id(node->value.var_decl.id, next_indent, mode, stream);
        fputc('\n', stream);
        hhg_node_fprint_core(node->value.var_decl.value, next_indent, mode, stream);
        break;
    case HHG_NODE_BLOCK:
        size_t len = arrlenu(node->value.block.body);
        for (size_t i = 0; i < len; i++)
            hhg_node_fprint_core(node->value.block.body[i], next_indent, mode, stream);
        break;
    case HHG_NODE_IF:
        hhg_node_fprint_core(node->value.if_stmt.cond, next_indent, mode, stream);
        hhg_node_fprint_core(node->value.if_stmt.if_body, next_indent, mode, stream);
        hhg_node_fprint_core(node->value.if_stmt.else_body, next_indent, mode, stream);
        break;
    case HHG_NODE_WHILE:
        hhg_node_fprint_core(node->value.while_stmt.cond, next_indent, mode, stream);
        hhg_node_fprint_core(node->value.while_stmt.body, next_indent, mode, stream);
        break;
    case HHG_NODE_RETURN:
        hhg_node_fprint_core(node->value.return_stmt.value, next_indent, mode, stream);
        break;
    case HHG_NODE_FOR:
        hhg_node_fprint_id(node->value.for_stmt.id, next_indent, mode, stream);
        fputc('\n', stream);
        hhg_node_fprint_core(node->value.for_stmt.iter, next_indent, mode, stream);
        break;
    case HHG_NODE_INT_LIT:
        hhg_node_fprint_str(node->value.int_lit.str, next_indent, stream);
        break;
    case HHG_NODE_FLOAT_LIT:
        hhg_node_fprint_str(node->value.float_lit.str, next_indent, stream);
        break;
    case HHG_NODE_STR_LIT:
        hhg_node_fprint_str(node->value.str_lit.str, next_indent, stream);
        break;
    case HHG_NODE_BOOL_LIT:
        hhg_node_fprint_str(
            node->value.bool_lit.value ? "true" : "false",
            next_indent, 
            stream
        );
        break;
    case HHG_NODE_CHAR_LIT:
        hhg_node_fprint_str(node->value.char_lit.str, next_indent, stream);
        break;
    case HHG_NODE_ARR_LIT:
        size_t len = arrlenu(node->value.arr_lit.elems);
        for (size_t i = 0; i < len; i++)
            hhg_node_fprint_core(
                node->value.arr_lit.elems[i],
                next_indent,
                mode,
                stream
            );
        break;
    case HHG_NODE_ADD:
    case HHG_NODE_SUB:
    case HHG_NODE_MUL:
    case HHG_NODE_DIV:
    case HHG_NODE_MOD:
    case HHG_NODE_BIT_AND:
    case HHG_NODE_BIT_OR:
    case HHG_NODE_BIT_XOR:
    case HHG_NODE_BIT_NOT:
    case HHG_NODE_LSHIFT:
    case HHG_NODE_RSHIFT:
    case HHG_NODE_ADD_EQ:
    case HHG_NODE_SUB_EQ:
    case HHG_NODE_MUL_EQ:
    case HHG_NODE_DIV_EQ:
    case HHG_NODE_MOD_EQ:
    case HHG_NODE_BIT_AND_EQ:
    case HHG_NODE_BIT_OR_EQ:
    case HHG_NODE_BIT_XOR_EQ:
    case HHG_NODE_LSHIFT_EQ:
    case HHG_NODE_RSHIFT_EQ:
    case HHG_NODE_EQ_EQ:
    case HHG_NODE_NOT_EQ:
    case HHG_NODE_LT:
    case HHG_NODE_LT_EQ:
    case HHG_NODE_GT:
    case HHG_NODE_GT_EQ:
    case HHG_NODE_AND:
    case HHG_NODE_OR:
    case HHG_NODE_NOT:
        hhg_node_fprint_core(node->value.expr.left, next_indent, mode, stream);
        hhg_node_fprint_core(node->value.expr.right, next_indent, mode, stream);
        break;
    case HHG_NODE_NEG:
    case HHG_NODE_BIT_NOT:
    case HHG_NODE_REF:
    case HHG_NODE_DEREF:
        hhg_node_fprint_core(node->value.unary.opnd, next_indent, mode, stream);
        break;
    case HHG_NODE_ARR_IDX:
        hhg_node_fprint_core(node->value.arr_idx.arr, next_indent, mode, stream);
        hhg_node_fprint_core(node->value.arr_idx.idx, next_indent, mode, stream);
        break;
    case HHG_NODE_FN_CALL:
        hhg_node_fprint_core(node->value.fn_call.fn, next_indent, mode, stream);
        for (size_t i = 0; i < arrlenu(node->value.fn_call.args); i++)
            hhg_node_fprint_core(node->value.fn_call.args[i], next_indent, mode, stream);
        break;
    case HHG_NODE_ASSIGN:
        hhg_node_fprint_core(node->value.assign.left, next_indent, mode, stream);
        hhg_node_fprint_core(node->value.assign.right, next_indent, mode, stream);
        break;
    case HHG_NODE_RANGE:
        if (node->value.range.inclusive)
            hhg_node_fprint_str("..", next_indent, stream);
        else
            hhg_node_fprint_str("..=", next_indent, stream);
        hhg_node_fprint_core(node->value.range.start, next_indent, mode, stream);
        hhg_node_fprint_core(node->value.range.end, next_indent, mode, stream);
        break;
    default:
        hhg_fatal_error(
            "unhandled node type %t in hhg_node_fprint_core",
            node->type
        );
        break;
    }
}

static void hhg_node_fprint_indent(int32_t indent, FILE *stream)
{
    for (int32_t i = 0; i < indent; i++)
       fputc(' ', stream);
}

static void hhg_node_fprint_str(const char *str, int32_t indent, FILE *stream)
{
    if (str) {
        hhg_node_fprint_indent(indent, stream);
        fputs(str, stream);
        fputc('\n', stream);
    }
}

static void hhg_node_fprint_id(
    hhg_node_id_t id,
    int32_t indent,
    hhg_node_print_mode_t mode,
    FILE *stream
)
{
    hhg_node_fprint_indent(indent, stream);
    if (mode == HHG_NODE_PRINT_MODE_SYM) {
        fputs(id.sym->key, stream);
        fputc(': ', stream);
        hhg_type_fprint(id.sym->value.type, stream);
    } else
        fputs(id.str, stream);
}
