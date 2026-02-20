#include "cpp_gen.h"
#include "code_gen.h"
#include "cfg.h"
#include "mem.h"
#include "utils.h"

typedef struct hhg_cpp_gen_backend {
    hhg_code_gen_backend_t base;
    // cpp specific data here
} hhg_cpp_gen_backend_t;

hhg_code_gen_backend_t *hhg_cpp_gen_backend_new(hhg_arena_t *arena)
{
    hhg_cpp_gen_backend_t *backend = hhg_arena_malloc(arena, sizeof(hhg_cpp_gen_backend_t));
    *backend = (hhg_cpp_gen_backend_t) {
        .base.type = HHG_CFG_BACKEND_CPP,
    };
    return (hhg_code_gen_backend_t *)backend;
}

void hhg_cpp_gen_backend_free(hhg_code_gen_backend_t *backend)
{
    // nothing inside to free for now
    // backend allocated in arena
    HHG_UNUSED(backend);
}
