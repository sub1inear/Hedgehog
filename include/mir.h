#ifndef HHG_IR_H
#define HHG_IR_H

#include <stdint.h>

#include "node.h"
#include "type.h"
#include "mem.h"

typedef struct hhg_file_src hhg_file_src_t;
typedef struct hhg_file_range hhg_file_range_t;

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

    HHG_MIR_CMP,
    HHG_MIR_JUMP,
    HHG_MIR_BR,

    HHG_MIR_CALL,
    HHG_MIR_FUNC,
    HHG_MIR_RET,

    HHG_MIR_LOAD,
    HHG_MIR_STORE,

    HHG_MIR_ALLOC,
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
    HHG_MIR_STRUCT_INSERT,
    HHG_MIR_STRUCT_EXTRACT,

    HHG_MIR_ARR_INIT,
    HHG_MIR_ARR_GET,
    HHG_MIR_ARR_SET,

    HHG_MIR_LBL,
} hhg_mir_op_t;

typedef enum hhg_mir_cmp {
    HHG_MIR_EQ,
    HHG_MIR_NEQ,
    HHG_MIR_LT,
    HHG_MIR_LTE,
    HHG_MIR_GT,
    HHG_MIR_GTE,
} hhg_mir_cmp_t;

typedef union hhg_mir_cnst_value {
    int64_t si;
    uint64_t ui;
    float f;
    double d;
    char *str;
} hhg_mir_cnst_value_t;

typedef struct hhg_mir_cnst {
    hhg_mir_cnst_value_t value;
    hhg_base_type_t type;
} hhg_mir_cnst_t;

typedef union hhg_mir_opnd_value {
    int64_t lbl;
    int64_t reg;
    hhg_mir_cnst_t cnst;
    hhg_mir_cmp_t cmp;
} hhg_mir_opnd_value_t;

typedef struct hhg_mir_opnd {
    hhg_type_t *type;
    hhg_mir_opnd_value_t value;
} hhg_mir_opnd_t;

typedef struct hhg_mir_instr {
    hhg_mir_op_t op;
    hhg_mir_opnd_t *opnds;
    hhg_file_src_t *src;
    hhg_file_range_t *range;
} hhg_mir_instr_t;

typedef struct hhg_mir_ctx {
    int x; // placeholder
} hhg_mir_ctx_t;

typedef struct hhg_mir_gen {
    hhg_mir_ctx_t *ctx;
    hhg_arena_t *arena;
} hhg_mir_gen_t;

void hhg_mir_gen_init(hhg_mir_gen_t *gen, hhg_arena_t *arena);

hhg_mir_instr_t *hhg_mir_instr_new(hhg_mir_gen_t *gen, hhg_mir_op_t op);
hhg_mir_instr_t *hhg_mir_instr_new_opnd(
    hhg_mir_gen_t *gen,
    hhg_mir_op_t op,
    hhg_mir_opnd_t opnd
);

hhg_mir_instr_t *hhg_mir_gen_run(hhg_mir_gen_t *gen, hhg_node_t *prog);

void hhg_mir_gen_print(hhg_mir_gen_t *gen);

void hhg_mir_gen_del(hhg_mir_gen_t *gen);

void hhg_mir_instr_free(hhg_mir_instr_t *instr);

#endif
