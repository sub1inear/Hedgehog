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
    hhg_cpp_gen_backend_t *backend =
        hhg_arena_malloc(arena, sizeof(hhg_cpp_gen_backend_t));
    *backend = (hhg_cpp_gen_backend_t) {
        .base = {
            .type = HHG_CFG_BACKEND_CPP,
            .ext = "c",
        },
    };
    return (hhg_code_gen_backend_t *)backend;
}

void hhg_cpp_gen_backend_run(
    hhg_code_gen_backend_t *backend,
    hhg_code_gen_t *code_gen,
    hhg_mir_gen_t *mir_gen,
    FILE *file
)
{
    HHG_UNUSED(backend, code_gen, mir_gen);
    fputs("int main(int argc, char **argv) { printf(\"Hello, World!\\n\"); return 0; }\n", file);
}

void hhg_cpp_gen_backend_print(hhg_code_gen_backend_t *backend)
{
    // nothing to print for now
    HHG_UNUSED(backend);
}

void hhg_cpp_gen_backend_free(hhg_code_gen_backend_t *backend)
{
    // nothing inside to free for now
    // backend allocated in arena
    HHG_UNUSED(backend);
}
