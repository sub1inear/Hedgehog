#ifndef HHG_NODE_H
#define HHG_NODE_H

#include <stdint.h>

#include "token.h"

typedef hhg_token_type_t hhg_node_type_t;

typedef struct _hhg_node_t {
    hhg_node_type_t type;
    hhg_str_t str;
    struct _hhg_node_t *children;
} hhg_node_t;

void hhg_node_init(hhg_node_t *node, hhg_str_t str);

void hhg_node_print(hhg_node_t *node);

void hhg_node_del(hhg_node_t *node);

#endif