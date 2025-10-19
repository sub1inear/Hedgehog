#ifndef HHG_NODE_H
#define HHG_NODE_H

#include "token.h"
#include "sym.h"
#include "mem.h"

#define HHG_NODE_INDENT_START 0

#define HHG_NODE_START HHG_TOKEN_END

enum _hhg_node_type_t {
    HHG_NODE_BLOCK = HHG_NODE_START,
    HHG_NODE_ARG,
    HHG_NODE_FUNC_CALL,
};
typedef int hhg_node_type_t;

#define HHG_NODE_END (HHG_NODE_FUNC_CALL + 1)

typedef struct _hhg_node_t hhg_node_t;

typedef struct _hhg_expr_t {
    hhg_node_t *left;
    hhg_node_t *right;
} hhg_expr_t;

typedef struct _hhg_block_t {
    hhg_node_t **body;
} hhg_block_t;

typedef struct _hhg_id_t {
    char *id;
} hhg_id_t;

typedef struct _hhg_var_decl_t {
    char *id;
    hhg_node_t *expr;
} hhg_var_decl_t;

typedef struct _hhg_literal_t {
    char *str;
} hhg_literal_t;

typedef struct _hhg_if_t {
    hhg_node_t *cond;
    hhg_node_t *if_body;
    hhg_node_t *else_body;
} hhg_if_t;

typedef struct _hhg_while_t {
    hhg_node_t *cond;
    hhg_node_t *body;
} hhg_while_t;

typedef struct _hhg_ret_t {
    hhg_node_t *expr;
} hhg_ret_t;

typedef struct _hhg_arg_t {
    char *arg;
} hhg_arg_t;

typedef struct _hhg_func_decl_t {
    char *id;
    hhg_node_t **args;
    hhg_node_t *body;
} hhg_func_decl_t;

typedef struct _hhg_func_call_t {
    char *id;
    hhg_node_t **args;
} hhg_func_call_t;

typedef struct _hhg_class_decl_t {
    char *id;
    hhg_node_t **var_decls;
    hhg_node_t **func_decls;
} hhg_class_decl_t;

typedef union _hhg_node_value_t  {
    hhg_expr_t expr;
    hhg_block_t block;
    hhg_id_t id;
    hhg_var_decl_t var_decl;
    hhg_literal_t literal;
    hhg_if_t if_stmt;
    hhg_while_t while_stmt;
    hhg_ret_t ret;
    hhg_arg_t arg;
    hhg_func_decl_t func_decl;
    hhg_func_call_t func_call;
    hhg_class_decl_t class_decl;
} hhg_node_value_t;

struct _hhg_node_t {
    hhg_node_type_t type;
    hhg_node_value_t value;
    hhg_type_t value_type;
};

hhg_node_t *hhg_node_new(hhg_arena_t *arena, hhg_node_type_t type);

void hhg_node_print(hhg_node_t *node, int32_t indent);

#endif
