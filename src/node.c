#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include <stb_ds.h>

#include "node.h"
#include "token.h"
#include "mem.h"
#include "error.h"

#define HHG_NODE_INDENT_INC 4

static void hhg_node_print_indent(int32_t indent);
static void hhg_node_print_str(const char *str, int32_t indent);

hhg_node_t *hhg_node_new(hhg_arena_t *arena, hhg_node_type_t type)
{
    hhg_node_t *node = hhg_arena_malloc(arena, sizeof(hhg_node_t));

    // initialize type and set other fields to NULL portably
    *node = (hhg_node_t) { .type = type };

    hhg_type_init(&node->value_type);

    return node;
}


void hhg_node_print(hhg_node_t *node, int32_t indent)
{   
    if (node == NULL)
        return;
    hhg_node_print_indent(indent);

    hhg_token_type_print((hhg_token_type_t)node->type);
    putchar('\n');

    int32_t next_indent = indent + HHG_NODE_INDENT_INC;
    int32_t next_next_indent = next_indent + HHG_NODE_INDENT_INC;

    switch (node->type) {
    case HHG_NODE_BLOCK: {
        size_t len = arrlenu(node->value.block.body);
        for (size_t i = 0; i < len; i++)
            hhg_node_print(node->value.block.body[i], next_indent);
        break;
    }
    case HHG_NODE_ARG:
        hhg_node_print_str(node->value.arg.arg, next_indent);
        break;
    case HHG_TOKEN_ID:
        hhg_node_print_str(node->value.id.id, next_indent);
        break;
    case HHG_TOKEN_STRING_LITERAL:
    case HHG_TOKEN_INT_LITERAL:
    case HHG_TOKEN_FLOAT_LITERAL:
        hhg_node_print_str(node->value.literal.str, next_indent);
        break;
    case HHG_TOKEN_IF:
        hhg_node_print(node->value.if_stmt.cond, next_indent);
        hhg_node_print(node->value.if_stmt.if_body, next_indent);
        break;
    case HHG_TOKEN_WHILE:
        hhg_node_print(node->value.while_stmt.cond, next_indent);
        hhg_node_print(node->value.while_stmt.body, next_indent);
        break;
    case '=':
        hhg_node_print_str(node->value.var_decl.id, next_indent);
        hhg_node_print(node->value.var_decl.expr, next_indent);
        break;
    case HHG_TOKEN_DEF: {
        hhg_node_print_str("id", next_indent);
        hhg_node_print_str(node->value.func_decl.id, next_next_indent);

        hhg_node_print_str("args", next_indent);
        size_t len = arrlenu(node->value.func_decl.args);
        for (size_t i = 0; i < len; i++)
            hhg_node_print(node->value.func_decl.args[i], next_next_indent);

        hhg_node_print(node->value.func_decl.body, next_indent);
        break;
    }
    case HHG_TOKEN_CLASS: {
        hhg_node_print_str("id", next_indent);
        hhg_node_print_str(node->value.class_decl.id, next_next_indent);
        hhg_node_print_str("var decls", next_indent);
        size_t var_decls_len = arrlenu(node->value.class_decl.var_decls);
        for (size_t i = 0; i < var_decls_len; i++)
            hhg_node_print(node->value.class_decl.var_decls[i], next_next_indent);

        hhg_node_print_str("func decls", next_indent);
        size_t func_decls_len = arrlenu(node->value.class_decl.func_decls);
        for (size_t i = 0; i < func_decls_len; i++)
            hhg_node_print(node->value.class_decl.func_decls[i], next_next_indent);
        break;
    }
    case '.':
        hhg_node_print_str("id", next_indent);
        hhg_node_print_str(node->value.field_access.id, next_next_indent);
        hhg_node_print(node->value.field_access.next, next_indent);
        break;
    case HHG_NODE_FUNC_CALL: {
        hhg_node_print_str("id", next_indent);
        hhg_node_print_str(node->value.func_call.id, next_next_indent);

        hhg_node_print_str("args", next_indent);
        size_t len = arrlenu(node->value.func_call.args);
        for (size_t i = 0; i < len; i++)
            hhg_node_print(node->value.func_call.args[i], next_next_indent);
        break;
    }
    case HHG_TOKEN_TRUE:
    case HHG_TOKEN_FALSE:
        break;
    case HHG_NODE_ARR_LITERAL: {
        size_t len = arrlenu(node->value.arr_literal.elems);
        for (size_t i = 0; i < len; i++)
            hhg_node_print(node->value.arr_literal.elems[i], next_indent);
        break;
    }
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case '<':
    case '>':
    case '&':
    case '^':
    case '|':
    case HHG_TOKEN_LSHIFT:
    case HHG_TOKEN_RSHIFT:
    case HHG_TOKEN_EQ:
    case HHG_TOKEN_NOT_EQ:
    case HHG_TOKEN_LT_EQ:
    case HHG_TOKEN_GT_EQ:
    case HHG_TOKEN_PLUS_EQ:
    case HHG_TOKEN_SUB_EQ:
    case HHG_TOKEN_MUL_EQ:
    case HHG_TOKEN_DIV_EQ:
    case HHG_TOKEN_MOD_EQ:
    case HHG_TOKEN_AND_EQ:
    case HHG_TOKEN_OR_EQ:
    case HHG_TOKEN_XOR_EQ:
    case HHG_TOKEN_LSHIFT_EQ:
    case HHG_TOKEN_RSHIFT_EQ:
    case HHG_TOKEN_INC:
    case HHG_TOKEN_DEC:
    case HHG_TOKEN_AND:
    case HHG_TOKEN_OR:
        hhg_node_print(node->value.expr.left, next_indent);
        hhg_node_print(node->value.expr.right, next_indent);
        break;
    default:
        hhg_fatal_error("unhandled node type %t in hhg_node_print", node->type);
        break;
    }
}

static void hhg_node_print_indent(int32_t indent)
{
    for (int32_t i = 0; i < indent; i++) {
        putchar(' ');
    }
}

static void hhg_node_print_str(const char *str, int32_t indent)
{
    if (str) {
        hhg_node_print_indent(indent);
        puts(str);
    }
}
