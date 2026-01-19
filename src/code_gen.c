#include <stdio.h>

#include "code_gen.h"
#include "cpp_gen.h"
#include "qbe_gen.h"
#include "msg.h"
#include "utils.h"

hhg_code_gen_backend_t *hhg_code_gen_backend_new(
    hhg_arena_t *arena,
    hhg_code_gen_backend_type_t type
)
{
    switch (type) {
    case HHG_CODE_GEN_CPP:
        return hhg_cpp_gen_backend_new(arena);
    case HHG_CODE_GEN_QBE:
        return hhg_qbe_gen_backend_new(arena);
    default:
        hhg_fatal_error("unknown code generation backend type: %d", type);
        return NULL;
    }
}

void hhg_code_gen_init(
    hhg_code_gen_t *gen,
    hhg_code_gen_backend_t *backend,
    const char *filename
)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
        hhg_fatal_error("failed to open output file: %s", filename);

    *gen = (hhg_code_gen_t) {
        .backend = backend,
        .out = file,
    };
}

void hhg_code_gen_run(hhg_code_gen_t *gen)
{
    HHG_UNUSED(gen);
}

void hhg_code_gen_backend_free(hhg_code_gen_backend_t *backend)
{
    switch (backend->type) {
    case HHG_CODE_GEN_CPP:
        hhg_cpp_gen_backend_free(backend);
        break;
    case HHG_CODE_GEN_QBE:
        hhg_qbe_gen_backend_free(backend);
        break;
    default:
        hhg_fatal_error("unknown code generation backend type: %d", backend->type);
        break;
    }
}

void hhg_code_gen_del(hhg_code_gen_t *gen)
{
    fclose(gen->out);
}