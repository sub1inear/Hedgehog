#ifndef HHG_NODE_H
#define HHG_NODE_H

#include <stdbool.h>
#include <stdio.h>

#include "token.h"
#include "sym.h"
#include "file_range.h"

typedef struct hhg_file_src hhg_file_src_t;
typedef struct hhg_type hhg_type_t;

#define HHG_NODE_START HHG_TOKEN_END
#define HHG_NODE_END (HHG_NODE_OBJ_INIT + 1)

enum hhg_node_type {
    HHG_NODE_BLOCK = HHG_NODE_START,
    HHG_NODE_PARAM,
    HHG_NODE_FUNC_CALL,
    HHG_NODE_ARR_LITERAL,
    HHG_NODE_OBJ_INIT,
};
typedef int hhg_node_type_t;

typedef struct hhg_node hhg_node_t;

typedef struct hhg_expr {
    hhg_node_t *left;
    hhg_node_t *right;
} hhg_expr_t;

typedef struct hhg_block {
    hhg_node_t **body;
} hhg_block_t;

typedef union hhg_id {
    hhg_sym_t *sym;
    char *str;
} hhg_id_t;

typedef struct hhg_var_decl {
    bool first;
    hhg_id_t id;
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

typedef struct hhg_param {
    hhg_id_t id;
} hhg_param_t;

typedef struct hhg_func_decl {
    hhg_id_t id;
    hhg_node_t **params;
    hhg_node_t *body;
} hhg_func_decl_t;

typedef struct hhg_func_call {
    hhg_id_t id;
    hhg_node_t **args;
} hhg_func_call_t;

typedef struct hhg_class_decl {
    hhg_id_t id;
    hhg_node_t **var_decls;
    hhg_node_t **func_decls;
} hhg_class_decl_t;

typedef struct hhg_field_access_t {
    char *str;
    hhg_node_t *next;
} hhg_field_access_t;

typedef struct hhg_obj_init {
    hhg_node_t **args;
} hhg_obj_init_t;

typedef union hhg_node_value  {
    // +, -, *, /, %, <, >, &, ^, |
    // HHG_TOKEN_LSHIFT, HHG_TOKEN_RSHIFT, HHG_TOKEN_EQ, HHG_TOKEN_NOT_EQ
    // HHG_TOKEN_LT_EQ, HHG_TOKEN_GT_EQ, HHG_TOKEN_PLUS_EQ,
    // HHG_TOKEN_SUB_EQ, HHG_TOKEN_MUL_EQ, HHG_TOKEN_DIV_EQ,
    // HHG_TOKEN_MOD_EQ, HHG_TOKEN_AND_EQ, HHG_TOKEN_OR_EQ,
    // HHG_TOKEN_XOR_EQ, HHG_TOKEN_LSHIFT_EQ, HHG_TOKEN_RSHIFT_EQ,
    // HHG_TOKEN_INC, HHG_TOKEN_DEC, HHG_TOKEN_AND, HHG_TOKEN_OR,
    hhg_expr_t expr;
    // HHG_NODE_BLOCK
    hhg_block_t block;
    // HHG_TOKEN_ID
    hhg_id_t id;
    // =
    hhg_var_decl_t var_decl;
    // HHG_TOKEN_STRING_LITERAL, HHG_TOKEN_INT_LITERAL,
    // HHG_TOKEN_FLOAT_LITERAL, HHG_TOKEN_TRUE, HHG_TOKEN_FALSE
    hhg_literal_t literal;
    // HHG_NODE_ARR_LITERAL
    hhg_arr_literal_t arr_literal;
    // HHG_TOKEN_IF
    hhg_if_t if_stmt;
    // HHG_TOKEN_WHILE
    hhg_while_t while_stmt;
    // HHG_TOKEN_RETURN
    hhg_ret_t ret;
    // HHG_NODE_PARAM
    hhg_param_t param;
    // HHG_TOKEN_DEF
    hhg_func_decl_t func_decl;
    // HHG_NODE_FUNC_CALL
    hhg_func_call_t func_call;
    // HHG_TOKEN_CLASS
    hhg_class_decl_t class_decl;
    // .
    hhg_field_access_t field_access;
    // HHG_NODE_OBJ_INIT
    hhg_obj_init_t obj_init;
} hhg_node_value_t;

struct hhg_node {
    hhg_type_t *value_type;
    hhg_file_src_t *src;
    hhg_node_value_t value;
    hhg_file_range_t range;
    hhg_node_type_t type;
};

enum hhg_node_print_mode {
    HHG_NODE_PRINT_MODE_SYM,
    HHG_NODE_PRINT_MODE_NO_SYM,
};
typedef int hhg_node_print_mode_t; // for printing

void hhg_node_type_print(hhg_node_type_t type);
void hhg_node_type_fprint(hhg_node_type_t type, FILE *stream);

hhg_node_t *hhg_node_new(
    hhg_arena_t *arena,
    hhg_node_type_t type,
    hhg_file_src_t *src
);

void hhg_node_print(hhg_node_t *node, hhg_node_print_mode_t mode);
void hhg_node_fprint(
    hhg_node_t *node,
    hhg_node_print_mode_t mode,
    FILE *stream
);

void hhg_node_free(hhg_node_t *node);

#endif
