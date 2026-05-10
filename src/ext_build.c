#include <stdbool.h>

#include <stb_ds.h>

#include "ext_build.h"
#include "code_gen.h"
#include "cfg.h"
#include "msg.h"
#include "utils.h"

#define hhg_ext_build_msg(ext_build, type, ...) \
    hhg_basic_msg(                              \
        ext_build->msg_ctx,                     \
        type,                                   \
        __VA_ARGS__                             \
    )
#define hhg_ext_build_error(ext_build, ...) \
    hhg_ext_build_msg(ext_build, HHG_MSG_ERROR, __VA_ARGS__)
#define hhg_ext_build_warning(ext_build, ...) \
    hhg_ext_build_msg(ext_build, HHG_MSG_WARNING, __VA_ARGS__)
#define hhg_ext_build_info(ext_build, ...) \
    hhg_ext_build_msg(ext_build, HHG_MSG_INFO, __VA_ARGS__)

static bool hhg_ext_build_run_core(
    hhg_ext_build_t *ext_build,
    hhg_code_gen_t *code_gen,
    const char *project_name
);

void hhg_ext_build_init(
    hhg_ext_build_t *ext_build,
    hhg_msg_ctx_t *msg_ctx
)
{
    *ext_build = (hhg_ext_build_t) {
        .msg_ctx = msg_ctx,
    };
}

void hhg_ext_build_run(
    hhg_ext_build_t *ext_build,
    hhg_code_gen_t *code_gen,
    const char *project_name
)
{
    if (hhg_ext_build_run_core(ext_build, code_gen, project_name)) {
        hhg_ext_build_error(
            ext_build,
            "external build failed for %s",
            project_name
        );
    }
}
static bool hhg_ext_build_run_core(
    hhg_ext_build_t *ext_build,
    hhg_code_gen_t *code_gen,
    const char *project_name
)
{
    HHG_UNUSED(ext_build);
    
    bool result = false;

    size_t size = arrlenu(code_gen->filenames);
    for (size_t i = 0; i < size; i++) {
        const char *filename = code_gen->filenames[i];
        switch (code_gen->backend->type) {
        case HHG_CFG_BACKEND_CPP:
            result |= hhg_utils_system("tcc %s -o %s" HHG_UTILS_EXEC_EXT, filename, project_name) == EXIT_FAILURE;
            break;
        case HHG_CFG_BACKEND_QBE:
            result |= hhg_utils_system("qbe %s -o %s.ssa", filename, project_name) == EXIT_FAILURE;
            break;
        default:
            hhg_compiler_error("unknown code generation backend type: %i", code_gen->backend->type);
            return true;
        }
    }
    return result;
}
