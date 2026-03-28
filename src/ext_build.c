#include <stdbool.h>

#include <stb_ds.h>

#include "ext_build.h"
#include "code_gen.h"
#include "cfg.h"
#include "msg.h"
#include "utils.h"

#define hhg_ext_build_msg(ext_build, type, ...) \
    hhg_basic_msg(                      \
        ext_build->msg_ctx,             \
        type,                           \
        __VA_ARGS__                     \
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
    hhg_cfg_backend_t backend
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
    hhg_cfg_backend_t backend
)
{
    if (hhg_ext_build_run_core(ext_build, code_gen, backend)) {
        hhg_ext_build_error(
            ext_build,
            "external build failed for %s",
            code_gen->filename
        );
    }
}
static bool hhg_ext_build_run_core(
    hhg_ext_build_t *ext_build,
    hhg_code_gen_t *code_gen,
    hhg_cfg_backend_t backend
)
{
    HHG_UNUSED(ext_build);
    switch (backend) {
    case HHG_CFG_BACKEND_CPP:
        return hhg_utils_system("g++ %s -o %s", code_gen->filename, code_gen->module) == EXIT_FAILURE;
    case HHG_CFG_BACKEND_QBE:
        return hhg_utils_system("qbe %s", code_gen->filename) == EXIT_FAILURE;
    default:
        hhg_compiler_error("unknown code generation backend type: %i", backend);
        return true;
    }
}
