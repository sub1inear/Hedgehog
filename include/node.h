#ifndef HHG_NODE_H
#define HHG_NODE_H

#include "token.h"
#include "sym.h"
#include "mem.h"

#define HHG_NODE_INDENT_START 0

#define HHG_NODE_START HHG_TOKEN_END

enum hhg_node_type{
    HHG_NODE_BLOCK = HHG_NODE_START,
    HHG_NODE_ARG,
    HHG_NODE_FUNC_CALL,
    HHG_NODE_ARR_LITERAL,
    HHG_NODE_OBJ_INIT,
};
typedef int hhg_node_type_t;

#define HHG_NODE_END (HHG_NODE_FUNC_CALL + 1)

typedef struct hhg_node hhg_node_t;

typedef struct hhg_expr {
    hhg_node_t *left;
    hhg_node_t *right;
} hhg_expr_t;

typedef struct hhg_block {
    hhg_node_t **body;
} hhg_block_t;

typedef struct hhg_id {
    char *id;
} hhg_id_t;

typedef struct hhg_var_decl {
    char *id;
    hhg_node_t *expr;
} hhg_var_decl_t;

typedef struct hhg_literal {
    char *str;
} hhg_literal_t;

typedef struct hhg_arr_literal {
    hhg_node_t **elems;
} hhg_arr_literal_t;

typedef struct hhg_if {
    hhg_node_t *cond;
    hhg_node_t *if_body;
    hhg_node_t *else_body;
} hhg_if_t;

typedef struct hhg_while {
    hhg_node_t *cond;
    hhg_node_t *body;
} hhg_while_t;

typedef struct hhg_ret {
    hhg_node_t *expr;
} hhg_ret_t;

typedef struct hhg_arg {
    char *arg;
} hhg_arg_t;

typedef struct hhg_func_decl {
    char *id;
    hhg_node_t **args;
    hhg_node_t *body;
} hhg_func_decl_t;

typedef struct hhg_func_call {
    char *id;
    hhg_node_t **args;
} hhg_func_call_t;

typedef struct hhg_class_decl {
    char *id;
    hhg_node_t **var_decls;
    hhg_node_t **func_decls;
} hhg_class_decl_t;

typedef struct hhg_field_access_t {
    char *id;
    hhg_node_t *next;
} hhg_field_access_t;

typedef struct hhg_obj_init {
    hhg_node_t **args;
} hhg_obj_init_t;

typedef union hhg_node_value  {
    hhg_expr_t expr;                 // +, -, *, /, %, ==, !=, <, <=, >, >=, &&, ||
    hhg_block_t block;               // HHG_NODE_BLOCK
    hhg_id_t id;                     // HHG_TOKEN_ID
    hhg_var_decl_t var_decl;         // =
    hhg_literal_t literal;           // HHG_TOKEN_STRING_LITERAL, HHG_TOKEN_INT_LITERAL, HHG_TOKEN_FLOAT_LITERAL, HHG_TOKEN_TRUE, HHG_TOKEN_FALSE
    hhg_arr_literal_t arr_literal;   // HHG_NODE_ARR_LITERAL
    hhg_if_t if_stmt;                // HHG_TOKEN_IF
    hhg_while_t while_stmt;          // HHG_TOKEN_WHILE
    hhg_ret_t ret;                   // HHG_TOKEN_RETURN
    hhg_arg_t arg;                   // HHG_NODE_ARG
    hhg_func_decl_t func_decl;       // HHG_TOKEN_DEF
    hhg_func_call_t func_call;       // HHG_NODE_FUNC_CALL
    hhg_class_decl_t class_decl;     // HHG_TOKEN_CLASS
    hhg_field_access_t field_access; // .
    hhg_obj_init_t obj_init;         // HHG_NODE_OBJ_INIT
} hhg_node_value_t;

struct hhg_node {
    hhg_node_type_t type;
    hhg_node_value_t value;
    hhg_type_t value_type;
};

hhg_node_t *hhg_node_new(hhg_arena_t *arena, hhg_node_type_t type);

void hhg_node_print(hhg_node_t *node, int32_t indent);

#endif
