#ifndef HHG_MIR_GEN_H
#define HHG_MIR_GEN_H

#include "mir.h"
#include "mem.h" // hhg_arena_t is based off arena_t so can't forward declare
#include "type.h"

typedef struct hhg_mir_gen_ctx {
    bool in_global_scope;
    hhg_mir_func_t *func;
    hhg_mir_local_t *local_tab;
} hhg_mir_gen_ctx_t;

typedef struct hhg_mir_gen {
    hhg_mir_func_t *funcs;

    hhg_mir_gen_ctx_t *ctx;
    hhg_sym_t **global_arr;
    hhg_type_t **tmp_arr;

    hhg_mir_reg_t reg_count;
    hhg_mir_lbl_t lbl_count;

    hhg_arena_t *arena;
} hhg_mir_gen_t;

void hhg_mir_gen_init(hhg_mir_gen_t *gen, hhg_arena_t *arena);

void hhg_mir_gen_run(hhg_mir_gen_t *gen, hhg_node_t *prog);

void hhg_mir_gen_print(hhg_mir_gen_t *gen);

void hhg_mir_gen_del(hhg_mir_gen_t *gen);

#endif
