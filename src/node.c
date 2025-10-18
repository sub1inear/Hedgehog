#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include <stb_ds.h>

#include "node.h"
#include "token.h"
#include "mem.h"

#define HHG_NODE_INDENT_INC 4

static void hhg_node_print_indent(int32_t indent);
static void hhg_node_print_str(const char *str, int32_t indent);

hhg_node_t *hhg_node_new(hhg_node_type_t type)
{
    hhg_node_t *node = hhg_malloc(sizeof(hhg_node_t));

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
    default:
        hhg_node_print(node->value.expr.left, next_indent);
        hhg_node_print(node->value.expr.right, next_indent);
        break;
    }
}

void hhg_node_free(hhg_node_t *node)
{
    switch (node->type) {
    case HHG_TOKEN_STRING_LITERAL:
    case HHG_TOKEN_INT_LITERAL:
    case HHG_TOKEN_FLOAT_LITERAL:
        hhg_free(node->value.literal.str);
        break;
    default:
        break;
    }
    hhg_free(node);
}

static void hhg_node_print_indent(int32_t indent)
{
    for (int32_t i = 0; i < indent; i++) {
        putchar(' ');
    }
}

static void hhg_node_print_str(const char *str, int32_t indent)
{
    hhg_node_print_indent(indent);
    puts(str);
}
