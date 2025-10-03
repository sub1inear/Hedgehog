#ifndef HHG_NODE_H
#define HHG_NODE_H

#include "token.h"
#include "sym.h"

#define HHG_NODE_INDENT_START 0

#define HHG_NODE_START HHG_TOKEN_END

enum _hhg_node_type_t {
    HHG_NODE_BLOCK = HHG_NODE_START,
};
typedef int hhg_node_type_t;

#define HHG_NODE_END (HHG_NODE_BLOCK + 1)

typedef struct _hhg_node_t {
    hhg_node_type_t type;
    hhg_str_t str;
    struct _hhg_node_t **children;
} hhg_node_t;

hhg_node_t *hhg_node_new(hhg_node_type_t type, hhg_str_t str);

hhg_node_t *hhg_node_new_va(hhg_node_type_t type, hhg_str_t str, int32_t children, ...);

void hhg_node_print(hhg_node_t *node, int32_t indent);

void hhg_node_free(hhg_node_t *node);

#endif
