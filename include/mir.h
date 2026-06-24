#ifndef HHG_MIR_H
#define HHG_MIR_H

#include <inttypes.h>
#include <stdbool.h>

#include "node.h"
#include "token.h"
#include "type.h"

#define HHG_PRIreg PRId32
#define HHG_PRIlbl PRId32
#define HHG_PRIfield PRId32

typedef int32_t hhg_mir_reg_t;
typedef int32_t hhg_mir_lbl_t;
typedef int32_t hhg_mir_field_t;

typedef enum hhg_mir_op {
    HHG_MIR_ADD,
    HHG_MIR_SUB,
    HHG_MIR_MUL,
    HHG_MIR_DIV,
    HHG_MIR_MOD,

    HHG_MIR_SHL,
    HHG_MIR_LSHR,
    HHG_MIR_ASHR,

    HHG_MIR_AND,
    HHG_MIR_OR,
    HHG_MIR_XOR,
    HHG_MIR_NOT,
    HHG_MIR_NEG,

    HHG_MIR_ASSIGN,

    HHG_MIR_CMP,
    HHG_MIR_JUMP,
    HHG_MIR_BR,

    HHG_MIR_CALL,
    HHG_MIR_RET,

    HHG_MIR_LOAD,
    HHG_MIR_STORE,

    HHG_MIR_ALLOCA,
    HHG_MIR_MALLOC,
    HHG_MIR_REALLOC,
    HHG_MIR_FREE,

    HHG_MIR_ARC,

    HHG_MIR_MOVE,
    HHG_MIR_COPY,

    HHG_MIR_BORROW,
    HHG_MIR_REBORROW,
    HHG_MIR_RELEASE,

    HHG_MIR_CAST,

    HHG_MIR_STRUCT_INIT,
    HHG_MIR_STRUCT_LOAD,
    HHG_MIR_STRUCT_STORE,

    HHG_MIR_ARR_INIT,
    HHG_MIR_ARR_LOAD,
    HHG_MIR_ARR_STORE,
} hhg_mir_op_t;

typedef enum hhg_mir_cnst_type {
    HHG_MIR_CNST_SI,
    HHG_MIR_CNST_UI,
    HHG_MIR_CNST_F32,
    HHG_MIR_CNST_F64,
    HHG_MIR_CNST_BOOL,
    HHG_MIR_CNST_CHAR,
    HHG_MIR_CNST_STR,
} hhg_mir_cnst_type_t;

typedef union hhg_mir_cnst_value {
    int64_t si;
    uint64_t ui;
    float f;
    double d;
    bool b;
    char c;
    char *str;
} hhg_mir_cnst_value_t;

typedef struct hhg_mir_cnst {
    hhg_mir_cnst_value_t value;
    hhg_mir_cnst_type_t type;
} hhg_mir_cnst_t;

typedef enum hhg_mir_opnd_type {
    HHG_MIR_OPND_NONE,
    HHG_MIR_OPND_REG,
    HHG_MIR_OPND_CNST
} hhg_mir_opnd_type_t;

typedef union hhg_mir_opnd_value {
    hhg_mir_reg_t reg;
    hhg_mir_cnst_t cnst;
} hhg_mir_opnd_value_t;

typedef struct hhg_mir_opnd {
    hhg_mir_opnd_type_t type;
    hhg_mir_opnd_value_t value;
} hhg_mir_opnd_t;

typedef struct hhg_mir_expr {
    hhg_mir_opnd_t left;
    hhg_mir_opnd_t right;
    hhg_mir_reg_t dst;
} hhg_mir_expr_t;

typedef enum hhg_mir_cmp_type {
    HHG_MIR_CMP_EQ,
    HHG_MIR_CMP_NEQ,
    HHG_MIR_CMP_LT,
    HHG_MIR_CMP_LTE,
    HHG_MIR_CMP_GT,
    HHG_MIR_CMP_GTE,
} hhg_mir_cmp_type_t;

typedef struct hhg_mir_cmp {
    hhg_mir_cmp_type_t cmp;
    hhg_mir_opnd_t left;
    hhg_mir_opnd_t right;
    hhg_mir_reg_t dst;
} hhg_mir_cmp_t;

typedef struct hhg_mir_jmp {
    hhg_mir_lbl_t lbl;
} hhg_mir_jmp_t;

typedef struct hhg_mir_br {
    hhg_mir_reg_t cond;
    hhg_mir_lbl_t lbl_true;
    hhg_mir_lbl_t lbl_false;
} hhg_mir_br_t;

typedef struct hhg_mir_call {
    hhg_sym_t *func;
    hhg_mir_opnd_t *args;
    hhg_mir_reg_t dst;
} hhg_mir_call_t;

typedef struct hhg_mir_ret {
    hhg_mir_opnd_t value;
} hhg_mir_ret_t;

typedef struct hhg_mir_load {
    hhg_sym_t *src;
    hhg_mir_reg_t dst;
} hhg_mir_load_t;

typedef struct hhg_mir_store {
    hhg_mir_opnd_t src;
    hhg_sym_t *dst;
} hhg_mir_store_t;

typedef struct hhg_mir_alloca {
    hhg_type_t *type;
    hhg_mir_reg_t dst;
} hhg_mir_alloca_t;

typedef struct hhg_mir_malloc {
    hhg_mir_opnd_t size;
    hhg_mir_reg_t dst;
} hhg_mir_malloc_t;

typedef struct hhg_mir_realloc {
    hhg_mir_reg_t ptr;
    hhg_mir_opnd_t size;
    hhg_mir_reg_t dst;
} hhg_mir_realloc_t;

typedef struct hhg_mir_free {
    hhg_mir_reg_t ptr;
} hhg_mir_free_t;

typedef struct hhg_mir_arc {
    hhg_mir_reg_t ptr;
} hhg_mir_arc_t;

typedef struct hhg_mir_move {
    hhg_mir_reg_t src;
    hhg_mir_reg_t dst;
} hhg_mir_move_t;

typedef struct hhg_mir_copy {
    hhg_mir_opnd_t src;
    hhg_mir_reg_t dst;
} hhg_mir_copy_t;

typedef struct hhg_mir_borrow {
    hhg_mir_reg_t src;
    hhg_mir_reg_t dst;
} hhg_mir_borrow_t;

typedef struct hhg_mir_reborrow {
    hhg_mir_reg_t src;
    hhg_mir_reg_t dst;
} hhg_mir_reborrow_t;

typedef struct hhg_mir_release {
    hhg_mir_reg_t ptr;
} hhg_mir_release_t;

typedef struct hhg_mir_cast {
    hhg_mir_reg_t src;
    hhg_type_t *dest_type;
    hhg_mir_reg_t dst;
} hhg_mir_cast_t;

typedef struct hhg_mir_struct_init {
    hhg_sym_t *type;
    hhg_mir_opnd_t *fields;
    hhg_mir_reg_t dst;
} hhg_mir_struct_init_t;

typedef struct hhg_mir_struct_load {
    hhg_mir_reg_t ptr;
    hhg_mir_field_t field_idx;
    hhg_mir_reg_t dst;
} hhg_mir_struct_load_t;

typedef struct hhg_mir_struct_store {
    hhg_mir_opnd_t ptr;
    hhg_mir_field_t field_idx;
    hhg_mir_opnd_t src;
} hhg_mir_struct_store_t;

typedef struct hhg_mir_arr_init {
    hhg_type_t *elem_type;
    hhg_mir_opnd_t *elems;
    hhg_mir_reg_t dst;
} hhg_mir_arr_init_t;

typedef struct hhg_mir_arr_load {
    hhg_mir_reg_t arr;
    hhg_mir_reg_t idx;
    hhg_mir_reg_t dst;
} hhg_mir_arr_load_t;

typedef struct hhg_mir_arr_store {
    hhg_mir_reg_t arr;
    hhg_mir_opnd_t idx;
    hhg_mir_opnd_t src;
} hhg_mir_arr_store_t;

typedef union hhg_mir_value {
    hhg_mir_expr_t expr;
    hhg_mir_cmp_t cmp;
    hhg_mir_jmp_t jmp;
    hhg_mir_br_t br;
    hhg_mir_call_t call;
    hhg_mir_ret_t ret;
    hhg_mir_load_t load;
    hhg_mir_store_t store;
    hhg_mir_alloca_t alloca;
    hhg_mir_malloc_t malloc;
    hhg_mir_realloc_t realloc;
    hhg_mir_free_t free;
    hhg_mir_arc_t arc;
    hhg_mir_move_t move;
    hhg_mir_copy_t copy;
    hhg_mir_borrow_t borrow;
    hhg_mir_reborrow_t reborrow;
    hhg_mir_release_t release;
    hhg_mir_cast_t cast;
    hhg_mir_struct_init_t struct_init;
    hhg_mir_struct_load_t struct_load;
    hhg_mir_struct_store_t struct_store;
    hhg_mir_arr_init_t arr_init;
    hhg_mir_arr_load_t arr_load;
    hhg_mir_arr_store_t arr_store;
} hhg_mir_value_t;

typedef struct hhg_mir_instr {
    hhg_mir_op_t op;
    hhg_mir_value_t *value;
    hhg_node_t *node;
} hhg_mir_instr_t;

typedef struct hhg_mir_func {
    hhg_sym_t *sym;
    hhg_mir_instr_t *instrs;
} hhg_mir_func_t;

typedef struct hhg_mir_local {
    hhg_sym_t *key;
    hhg_mir_reg_t value;
} hhg_mir_local_t;

void hhg_mir_opnd_print(hhg_mir_opnd_t *opnd);

void hhg_mir_instr_print(hhg_mir_instr_t *instr);

void hhg_mir_func_print(hhg_mir_func_t *func);

void hhg_mir_instr_free(hhg_mir_instr_t *instr);

void hhg_mir_func_free(hhg_mir_func_t *func);

#endif
