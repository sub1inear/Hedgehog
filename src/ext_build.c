#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

#include <stb_ds.h>
#include <fs.h>

#include "ext_build.h"
#include "code_gen.h"
#include "mem.h"
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

typedef struct hhg_ext_build_cxx_data {
    const char *cxx;
    const char *out_flag;
    const char *release_flag;
} hhg_ext_build_cxx_data_t;

static const hhg_ext_build_cxx_data_t ext_build_cxx_data[] = {
    { "gcc", "-o", "-O2" },
    { "clang", "-o", "-O2" },
    { "cl", "/Fe:", "/O2" },
};

void hhg_ext_build_init(
    hhg_ext_build_t *ext_build,
    const char *cxx,
    hhg_msg_ctx_t *msg_ctx,
    hhg_arena_t *arena
)
{
    if (cxx == NULL) {
        cxx = getenv("CXX");
        if (cxx == NULL)
            hhg_fatal_error("please specify a C++ compiler with --cxx or the CXX environment variable");
    }
    
    const char *cxx_basename = fs_basename(cxx);

    size_t i;
    for (i = 0; i < HHG_ARR_LEN(ext_build_cxx_data); i++)
        if (strcmp(cxx_basename, ext_build_cxx_data[i].cxx) == 0)
            break;

    if (i == HHG_ARR_LEN(ext_build_cxx_data))
        hhg_fatal_error("unsupported C++ compiler: %s", cxx);
    

    *ext_build = (hhg_ext_build_t) {
        .cxx = cxx,
        .cxx_data = &ext_build_cxx_data[i],
        .msg_ctx = msg_ctx,
        .arena = arena,
    };
}

void hhg_ext_build_run(
    hhg_ext_build_t *ext_build,
    hhg_code_gen_t *code_gen,
    const char *filename,
    const char *orig_filename,
    const char *out,
    bool release
)
{
    HHG_UNUSED(code_gen);

    const char **argv = NULL;
    arrput(argv, ext_build->cxx);
    arrput(argv, filename);
    arrput(argv, ext_build->cxx_data->out_flag);

    if (out == NULL)
        out = hhg_utils_file_to_exec(ext_build->arena, orig_filename);
    arrput(argv, out);

    if (release)
        arrput(argv, ext_build->cxx_data->release_flag);

    hhg_str_t stdouterr;
    hhg_str_init(&stdouterr);

    int exit_code = hhg_utils_spawn(argv, &stdouterr);
    
    arrfree(argv);

    if (exit_code != EXIT_SUCCESS)
        hhg_compiler_error(
            "external build failed: `%s` exited with code %i\noutput:\n%s",
            ext_build->cxx,
            exit_code,
            stdouterr.str
        );
    hhg_str_del(&stdouterr);
}
