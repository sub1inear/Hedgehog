#include <stdio.h>

#include <stb_ds.h>

#include "mir.h"
#include "mem.h"
#include "utils.h"


void hhg_mir_init(hhg_mir_gen_t *gen, hhg_arena_t *arena)
{
    *gen = (hhg_mir_gen_t){
        .instrs = NULL,
        .arena = arena,
    };
}

hhg_mir_instr_t *hhg_mir_gen_instr_new(hhg_mir_gen_t *gen, hhg_mir_op_t op)
{
    hhg_mir_instr_t *instr = hhg_arena_malloc(gen->arena, sizeof(hhg_mir_instr_t));
    *instr = (hhg_mir_instr_t){
        .op = op,
        .opnds = NULL,
        .src = NULL,
        .range = NULL,
    };
    return instr;
}

void hhg_mir_gen_run(hhg_mir_gen_t *gen, hhg_node_t *node)
{
    HHG_UNUSED(gen, node);
}

void hhg_mir_print(hhg_mir_gen_t *gen)
{
    HHG_UNUSED(gen);
}

void hhg_mir_del(hhg_mir_gen_t *gen)
{
    size_t len = arrlen(gen->instrs);
    for (size_t i = 0; i < len; i++) {
        hhg_mir_instr_t *instr = &gen->instrs[i];
        arrfree(instr->opnds);
    }
}