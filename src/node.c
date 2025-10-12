#include <stdio.h>
#include <stdbool.h>

#include <stb_ds.h>

#include "node.h"
#include "token.h"
#include "mem.h"

#define HHG_NODE_INDENT_INC 4

static void hhg_node_print_indent(int32_t indent);

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
    hhg_node_print_indent(indent);

    hhg_token_type_print((hhg_token_type_t)node->type);
    putchar('\n');

    hhg_type_print(node->value_type);
    putchar('\n');

    int32_t next_indent = indent + HHG_NODE_INDENT_INC;

    switch (node->type) {
    case HHG_NODE_BLOCK: {
        size_t len = arrlenu(node->value.block.body);
        for (size_t i = 0; i < len; i++)
            hhg_node_print(node->value.block.body[i], next_indent);
        break;
    }
    case HHG_NODE_ARG:
        hhg_node_print_indent(next_indent);
        puts(node->value.arg.arg);
        break;
    case HHG_TOKEN_ID:
        hhg_node_print_indent(next_indent);
        puts(node->value.id.id);
        break;
    case HHG_TOKEN_STRING_LITERAL:
    case HHG_TOKEN_INT_LITERAL:
    case HHG_TOKEN_FLOAT_LITERAL:
        hhg_node_print_indent(next_indent);
        puts(node->value.literal.str);
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
        hhg_node_print_indent(next_indent);
        puts(node->value.var_decl.id);
        hhg_node_print(node->value.var_decl.expr, next_indent);
        break;
    case HHG_TOKEN_DEF:
        hhg_node_print_indent(next_indent);
        puts(node->value.func_decl.id);
        size_t len = arrlenu(node->value.func_decl.args);
        for (size_t i = 0; i < len; i++)
            hhg_node_print(node->value.func_decl.args[i], next_indent);

        hhg_node_print(node->value.func_decl.body, next_indent);
        break;
    case HHG_TOKEN_TRUE:
        break;
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