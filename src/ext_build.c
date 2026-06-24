#include <stdbool.h>
#include <stdarg.h>

#include <stb_ds.h>

#include "ext_build.h"
#include "code_gen.h"
#include "cfg.h"
#include "str.h"
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

static void hhg_ext_build_run_tool(
    hhg_ext_build_t *ext_build,
    hhg_cfg_toolchain_tool_t *tool,
    hhg_cfg_t *cfg,
    const char **filenames,
    ...
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
    hhg_cfg_t *cfg
)
{
    HHG_UNUSED(ext_build);
    
    switch (code_gen->backend->type) {
    case HHG_CFG_BACKEND_CPP:
        hhg_ext_build_run_tool(
            ext_build,
            &cfg->toolchain.cpp.compiler,
            cfg,
            code_gen->filenames,
            "-o",
            cfg->project.name,
            NULL
        );
        break;
    case HHG_CFG_BACKEND_QBE:
        hhg_ext_build_run_tool(
            ext_build,
            &cfg->toolchain.qbe.compiler,
            cfg,
            code_gen->filenames,
            "-o",
            cfg->project.name,
            NULL
        );
        break;
    default:
        hhg_compiler_error(
            "unknown code generation backend type: %i",
            code_gen->backend->type
        );
        break;
    }
}

static void hhg_ext_build_run_tool(
    hhg_ext_build_t *ext_build,
    hhg_cfg_toolchain_tool_t *tool,
    hhg_cfg_t *cfg,
    const char **filenames,
    ...
)
{
    HHG_UNUSED(ext_build);

    va_list va;
    va_start(va, filenames);

    const char **argv = NULL;
    arrput(argv, tool->cmd);
    arrcat(argv, filenames);

    while (true) {
        const char *arg = va_arg(va, const char *);
        if (arg == NULL)
            break;
        arrput(argv, arg);
    }

    arrcat(argv, tool->flags);
    if (cfg->build.mode == HHG_CFG_BUILD_MODE_DEBUG)
        arrcat(argv, tool->debug.flags);
    else if (cfg->build.mode == HHG_CFG_BUILD_MODE_RELEASE)
        arrcat(argv, tool->release.flags);

    arrput(argv, NULL);

    hhg_str_t stdouterr;
    int exit_code = hhg_utils_spawn(argv, &stdouterr);
    
    arrfree(argv);

    if (exit_code != EXIT_SUCCESS)
        hhg_compiler_error(
            "external build failed: `%s` exited with code %i\noutput:\n%s",
            tool->cmd,
            exit_code,
            stdouterr.str
        );

    hhg_str_del(&stdouterr);
    
    va_end(va);
}
