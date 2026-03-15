#include "qbe_gen.h"
#include "code_gen.h"
#include "cfg.h"
#include "mem.h"
#include "utils.h"

typedef struct hhg_qbe_gen_backend {
    hhg_code_gen_backend_t base;
    // qbe specific data here
} hhg_qbe_gen_backend_t;

static void hhg_qbe_gen_start_func(hhg_code_gen_t *gen, hhg_sym_t *sym);
static void hhg_qbe_gen_emit_instr(hhg_code_gen_t *gen, hhg_mir_instr_t *instr);
static void hhg_qbe_gen_end_func(hhg_code_gen_t *gen, hhg_sym_t *sym);

hhg_code_gen_backend_t *hhg_qbe_gen_backend_new(hhg_arena_t *arena)
{
    hhg_qbe_gen_backend_t *backend = hhg_arena_malloc(arena, sizeof(hhg_qbe_gen_backend_t));
    *backend = (hhg_qbe_gen_backend_t) {
        .base = {
            .type = HHG_CFG_BACKEND_QBE,
            .vtbl = {
                .start_func = hhg_qbe_gen_start_func,
                .emit_instr = hhg_qbe_gen_emit_instr,
                .end_func = hhg_qbe_gen_end_func,
            },
            .ext = "ssa",
        },
    };
    return (hhg_code_gen_backend_t *)backend;
}

void hhg_qbe_gen_backend_print(hhg_code_gen_backend_t *backend)
{
    // nothing to print for now
    HHG_UNUSED(backend);
}

void hhg_qbe_gen_backend_free(hhg_code_gen_backend_t *backend)
{
    // nothing inside to free for now
    // backend allocated in arena
    HHG_UNUSED(backend);
}

static void hhg_qbe_gen_start_func(hhg_code_gen_t *gen, hhg_sym_t *sym)
{
    HHG_UNUSED(gen, sym);
}

static void hhg_qbe_gen_emit_instr(hhg_code_gen_t *gen, hhg_mir_instr_t *instr)
{
    HHG_UNUSED(gen, instr);
}

static void hhg_qbe_gen_end_func(hhg_code_gen_t *gen, hhg_sym_t *sym)
{
    HHG_UNUSED(gen, sym);
}
