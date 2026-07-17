#ifndef HHG_NODE_H
#define HHG_NODE_H

#include <stdbool.h>
#include <stdio.h>

#include "sym.h"
#include "file_range.h"

typedef struct hhg_file_src hhg_file_src_t;
typedef struct hhg_type hhg_type_t;

enum hhg_node_type {
    HHG_NODE_ID,
    HHG_NODE_BLOCK,

    HHG_NODE_FN_DECL,
    HHG_NODE_PARAM,
    HHG_NODE_VAR_DECL,

    HHG_NODE_IF,
    HHG_NODE_WHILE,
    HHG_NODE_RETURN,
    HHG_NODE_FOR,

    HHG_NODE_INT_LIT,
    HHG_NODE_FLOAT_LIT,
    HHG_NODE_STR_LIT,
    HHG_NODE_BOOL_LIT,
    HHG_NODE_CHAR_LIT,
    HHG_NODE_ARR_LIT,

    HHG_NODE_ADD,
    HHG_NODE_SUB,
    HHG_NODE_MUL,
    HHG_NODE_DIV,
    HHG_NODE_MOD,

    HHG_NODE_BIT_AND,
    HHG_NODE_BIT_OR,
    HHG_NODE_BIT_XOR,
    HHG_NODE_BIT_NOT,
    HHG_NODE_LSHIFT,
    HHG_NODE_RSHIFT,
    
    HHG_NODE_ADD_EQ,
    HHG_NODE_SUB_EQ,
    HHG_NODE_MUL_EQ,
    HHG_NODE_DIV_EQ,
    HHG_NODE_MOD_EQ,
    HHG_NODE_BIT_AND_EQ,
    HHG_NODE_BIT_OR_EQ,
    HHG_NODE_BIT_XOR_EQ,
    HHG_NODE_LSHIFT_EQ,
    HHG_NODE_RSHIFT_EQ,

    HHG_NODE_EQ_EQ,
    HHG_NODE_NOT_EQ,
    HHG_NODE_LT,
    HHG_NODE_LT_EQ,
    HHG_NODE_GT,
    HHG_NODE_GT_EQ,

    HHG_NODE_AND,
    HHG_NODE_OR,
    HHG_NODE_NOT,

    HHG_NODE_NEG,
    HHG_NODE_BIT_NOT,

    HHG_NODE_REF,
    HHG_NODE_DEREF,
    HHG_NODE_ARR_IDX,
    HHG_NODE_FN_CALL,
    HHG_NODE_ASSIGN,
    HHG_NODE_RANGE,
};
typedef int hhg_node_type_t; // for printing

typedef struct hhg_node hhg_node_t;

typedef union hhg_node_id {
    char *str;
    hhg_sym_t *sym;
} hhg_node_id_t;

typedef struct hhg_node_block {
    hhg_node_t **body;
} hhg_node_block_t;

typedef struct hhg_node_fn_decl {
    hhg_node_id_t id;
    hhg_node_t **params;
    hhg_type_t *return_type;
    hhg_node_t *body;
} hhg_node_fn_decl_t;

typedef struct hhg_node_param {
    hhg_node_id_t id;
} hhg_node_param_t;

typedef struct hhg_node_var_decl {
    hhg_node_id_t id;
    hhg_node_t *value;
} hhg_node_var_decl_t;

typedef struct hhg_node_if {
    hhg_node_t *cond;
    hhg_node_t *if_body;
    hhg_node_t *else_body;
} hhg_node_if_t;

typedef struct hhg_node_while {
    hhg_node_t *cond;
    hhg_node_t *body;
} hhg_node_while_t;

typedef struct hhg_node_return {
    hhg_node_t *value;
} hhg_node_return_t;

typedef struct hhg_node_for {
    hhg_node_id_t id;
    hhg_node_t *iter;
} hhg_node_for_t;

typedef struct hhg_node_int_lit {
    char *str;
} hhg_node_int_lit_t;

typedef struct hhg_node_float_lit {
    char *str;
} hhg_node_float_lit_t;

typedef struct hhg_node_str_lit {
    char *str;
} hhg_node_str_lit_t;

typedef struct hhg_node_char_lit {
    char *str;
} hhg_node_char_lit_t;

typedef struct hhg_node_bool_lit {
    bool value;
} hhg_node_bool_lit_t;

typedef struct hhg_node_arr_lit {
    hhg_node_t **elems;
} hhg_node_arr_lit_t;

typedef struct hhg_node_expr {
    hhg_node_t *left;
    hhg_node_t *right;
} hhg_node_expr_t;

typedef struct hhg_node_unary {
    hhg_node_t *opnd;
} hhg_node_unary_t;

typedef struct hhg_node_arr_idx {
    hhg_node_t *arr;
    hhg_node_t *idx;
} hhg_node_arr_idx_t;

typedef struct hhg_node_fn_call {
    hhg_node_t *fn;
    hhg_node_t **args;
} hhg_node_fn_call_t;

typedef struct hhg_node_assign {
    hhg_node_t *left;
    hhg_node_t *right;
} hhg_node_assign_t;

typedef struct hhg_node_range {
    hhg_node_t *start;
    hhg_node_t *end;
    bool inclusive;
} hhg_node_range_t;

typedef union hhg_node_value {
    hhg_node_id_t id;
    hhg_node_block_t block;
    hhg_node_fn_decl_t fn_decl;
    hhg_node_param_t param;
    hhg_node_var_decl_t var_decl;
    hhg_node_if_t if_stmt;
    hhg_node_while_t while_stmt;
    hhg_node_return_t return_stmt;
    hhg_node_for_t for_stmt;
    hhg_node_int_lit_t int_lit;
    hhg_node_float_lit_t float_lit;
    hhg_node_str_lit_t str_lit;
    hhg_node_char_lit_t char_lit;
    hhg_node_bool_lit_t bool_lit;
    hhg_node_arr_lit_t arr_lit;
    hhg_node_expr_t expr;
    hhg_node_unary_t unary;
    hhg_node_arr_idx_t arr_idx;
    hhg_node_fn_call_t fn_call;
    hhg_node_assign_t assign;
    hhg_node_range_t range;
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
    HHG_NODE_PRINT_MODE_STR,
};
typedef int hhg_node_print_mode_t; // for printing

void hhg_node_type_print(hhg_node_type_t type);
void hhg_node_type_fprint(hhg_node_type_t type, FILE *stream);
const char *hhg_node_type_to_str(hhg_node_type_t type);

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
