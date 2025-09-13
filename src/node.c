#include <stdio.h>

#include <stb_ds.h>

#include "node.h"
#include "token.h"

void hhg_node_init(hhg_node_t *node, hhg_str_t str)
{
    node->type = NONE;
    node->str = str;
    node->children = NULL;
}

void hhg_node_print(hhg_node_t *node)
{
    hhg_token_type_print((hhg_token_type_t)node->type);

    size_t len = arrlenu(node->children);
    for (size_t i = 0; i < len; i++) {
        fputs("    ", stdout);
        hhg_node_print(&node->children[i]);
    }
}

void hhg_node_del(hhg_node_t *node)
{
    if (node->children != NULL) {
        size_t len = arrlenu(node->children);
        for (size_t i = 0; i < len; i++)
            hhg_node_del(&node->children[i]);
        arrfree(node->children);
    }

    hhg_str_del(&node->str);
}