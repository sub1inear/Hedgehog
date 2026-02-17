#include <stdio.h>

#include "code_gen.h"
#include "cfg.h"
#include "cpp_gen.h"
#include "qbe_gen.h"
#include "mir.h"
#include "msg.h"
#include "utils.h"

hhg_code_gen_backend_t *hhg_code_gen_backend_new(
    hhg_arena_t *arena,
    hhg_cfg_build_backend_t type
)
{
    switch (type) {
    case HHG_CFG_BUILD_BACKEND_CPP:
        return hhg_cpp_gen_backend_new(arena);
    case HHG_CFG_BUILD_BACKEND_QBE:
        return hhg_qbe_gen_backend_new(arena);
    default:
        hhg_fatal_error("unknown code generation backend type: %i", type);
        return NULL;
    }
}

void hhg_code_gen_init(hhg_code_gen_t *gen, hhg_code_gen_backend_t *backend)
{
    *gen = (hhg_code_gen_t) {
        .backend = backend,
    };
}

void hhg_code_gen_set_file(hhg_code_gen_t *gen, const char *filename)
{
    gen->file = hhg_utils_fopen(filename, "w");
}

void hhg_code_gen_run(hhg_code_gen_t *code_gen, hhg_mir_gen_t *mir_gen)
{
    HHG_UNUSED(code_gen, mir_gen);
}

void hhg_code_gen_backend_free(hhg_code_gen_backend_t *backend)
{
    switch (backend->type) {
    case HHG_CFG_BUILD_BACKEND_CPP:
        hhg_cpp_gen_backend_free(backend);
        break;
    case HHG_CFG_BUILD_BACKEND_QBE:
        hhg_qbe_gen_backend_free(backend);
        break;
    default:
        hhg_fatal_error("unknown code generation backend type: %i", backend->type);
        break;
    }
}

void hhg_code_gen_del(hhg_code_gen_t *gen)
{
    fclose(gen->file);
}
