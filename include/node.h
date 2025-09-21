#ifndef HHG_NODE_H
#define HHG_NODE_H

#include <stdint.h>

#include "token.h"

#define HHG_NODE_INDENT_START 0

#define HHG_NODE_TYPE_START HHG_TOKEN_TYPE_END

enum {
    BLOCK = HHG_TOKEN_TYPE_END,
};
typedef int hhg_node_type_t;

#define HHG_NODE_TYPE_END (BLOCK + 1)

typedef struct _hhg_node_t {
    hhg_node_type_t type;
    hhg_str_t str;
    struct _hhg_node_t **children;
} hhg_node_t;

hhg_node_t *hhg_node_new(hhg_node_type_t type, hhg_str_t str);

hhg_node_t *hhg_node_new_va(hhg_node_type_t type, hhg_str_t str, int32_t children, ...);

void hhg_node_print(hhg_node_t *node, int32_t indent);

void hhg_node_del(hhg_node_t *node);

#endif
