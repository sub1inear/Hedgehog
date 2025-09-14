#include <stdio.h>
#include <stdarg.h>

#include <stb_ds.h>

#include "node.h"
#include "token.h"
#include "mem.h"

#define HHG_NODE_INDENT_INC 4

hhg_node_t *hhg_node_new(hhg_node_type_t type, hhg_str_t str)
{
    hhg_node_t *node = hhg_malloc(sizeof(hhg_node_t));
    node->type = type;
    node->str = str;
    node->children = NULL;
    return node;
}

hhg_node_t *hhg_node_new_va(hhg_node_type_t type, hhg_str_t str, int32_t children, ...)
{
    va_list va;
    va_start(va, children);

    hhg_node_t *node = hhg_node_new(type, str);

    for (int32_t i = 0; i < children; i++) {
        hhg_node_t *child = va_arg(va, hhg_node_t *);
        arrput(node->children, child);
    }

    va_end(va);

    return node;
}

static void hhg_node_print_indent(int32_t indent)
{
    for (int32_t i = 0; i < indent; i++)
    {
        putchar(' ');
    }
}

void hhg_node_print(hhg_node_t *node, int32_t indent)
{   
    hhg_node_print_indent(indent);
    hhg_token_type_print((hhg_token_type_t)node->type);
    putchar('\n');

    int32_t next_indent = indent + HHG_NODE_INDENT_INC;

    if (node->str.str != NULL) {
        hhg_node_print_indent(next_indent);
        printf("%s\n", node->str.str);
    }

    size_t len = arrlenu(node->children);
    for (size_t i = 0; i < len; i++)
        hhg_node_print(node->children[i], next_indent);
}

void hhg_node_del(hhg_node_t *node)
{
    if (node->children != NULL) {
        size_t len = arrlenu(node->children);
        for (size_t i = 0; i < len; i++)
            hhg_node_del(node->children[i]);
        arrfree(node->children);
    }
    // freeing NULL is safe
    hhg_str_del(&node->str);
}