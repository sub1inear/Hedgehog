#include "qbe_gen.h"
#include "code_gen.h"
#include "cfg.h"
#include "mem.h"
#include "utils.h"

typedef struct hhg_qbe_gen_backend {
    hhg_code_gen_backend_t base;
    // qbe specific data here
} hhg_qbe_gen_backend_t;

hhg_code_gen_backend_t *hhg_qbe_gen_backend_new(hhg_arena_t *arena)
{
    hhg_qbe_gen_backend_t *backend = hhg_arena_malloc(arena, sizeof(hhg_qbe_gen_backend_t));
    *backend = (hhg_qbe_gen_backend_t) {
        .base = {
            .type = HHG_CFG_BACKEND_QBE,
            .ext = "ssa",
        },
    };
    return (hhg_code_gen_backend_t *)backend;
}

void hhg_qbe_gen_backend_run(
    hhg_code_gen_backend_t *backend,
    hhg_code_gen_t *code_gen,
    hhg_mir_gen_t *mir_gen
)
{
    HHG_UNUSED(backend, mir_gen);
    fputs(
        "export function w $main(w %argc, l %argv) {\n"
        "    @start\n"
        "        ret 0\n"
        "    }\n"
        "}\n",
        code_gen->file
    );
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
