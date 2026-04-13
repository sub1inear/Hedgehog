#include <stdio.h>
#include <limits.h>
#include <string.h>

#include <stb_ds.h>
#include <fs.h>

#include "code_gen.h"
#include "cfg.h"
#include "cpp_gen.h"
#include "qbe_gen.h"
#include "mir.h"
#include "mem.h"
#include "msg.h"
#include "utils.h"

static hhg_code_gen_backend_t *hhg_code_gen_backend_new(
    hhg_arena_t *arena,
    hhg_cfg_backend_t type
);
static void hhg_code_gen_backend_free(hhg_code_gen_backend_t *backend);
static const char *hhg_code_gen_replace_ext(
    hhg_code_gen_t *gen,
    const char *in_filename
);

void hhg_code_gen_init(
    hhg_code_gen_t *gen,
    hhg_cfg_backend_t backend_type,
    const char *out_dir,
    hhg_arena_t *arena
)
{
    *gen = (hhg_code_gen_t) {
        .backend = hhg_code_gen_backend_new(arena, backend_type),
        .out_dir = out_dir,
        .arena = arena,
    };
    // create directory if it doesn't exist
    if (!fs_exist(out_dir))
        if (!fs_make_dir(out_dir))
            hhg_fatal_error("failed to create output directory: %s", out_dir);

}

void hhg_code_gen_run(
    hhg_code_gen_t *code_gen,
    hhg_mir_gen_t *mir_gen,
    const char *in_filename
)
{
    const char *out_filename = hhg_code_gen_replace_ext(code_gen, in_filename);
    FILE *file = hhg_utils_fopen(out_filename, "w");
    arrput(code_gen->filenames, out_filename);

    switch (code_gen->backend->type) {
    case HHG_CFG_BACKEND_CPP:
        hhg_cpp_gen_backend_run(code_gen->backend, code_gen, mir_gen, file);
        break;
    case HHG_CFG_BACKEND_QBE:
        hhg_qbe_gen_backend_run(code_gen->backend, code_gen, mir_gen, file);
        break;
    default:
        hhg_compiler_error(
            "unknown code generation backend type: %i",
            code_gen->backend->type
        );
        break;
    }
    
    fclose(file);
}

void hhg_code_gen_backend_print(hhg_code_gen_backend_t *backend)
{
    switch (backend->type) {
    case HHG_CFG_BACKEND_CPP:
        hhg_cpp_gen_backend_print(backend);
        break;
    case HHG_CFG_BACKEND_QBE:
        hhg_qbe_gen_backend_print(backend);
        break;
    default:
        hhg_compiler_error(
            "unknown code generation backend type: %i",
            backend->type
        );
        break;
    }
}

void hhg_code_gen_del(hhg_code_gen_t *gen)
{
    arrfree(gen->filenames);
    hhg_code_gen_backend_free(gen->backend);
}

static hhg_code_gen_backend_t *hhg_code_gen_backend_new(
    hhg_arena_t *arena,
    hhg_cfg_backend_t type
)
{
    switch (type) {
    case HHG_CFG_BACKEND_CPP:
        return hhg_cpp_gen_backend_new(arena);
    case HHG_CFG_BACKEND_QBE:
        return hhg_qbe_gen_backend_new(arena);
    default:
        hhg_compiler_error("unknown code generation backend type: %i", type);
        return NULL;
    }
}

static void hhg_code_gen_backend_free(hhg_code_gen_backend_t *backend)
{
    switch (backend->type) {
    case HHG_CFG_BACKEND_CPP:
        hhg_cpp_gen_backend_free(backend);
        break;
    case HHG_CFG_BACKEND_QBE:
        hhg_qbe_gen_backend_free(backend);
        break;
    default:
        hhg_compiler_error(
            "unknown code generation backend type: %i",
            backend->type
        );
        break;
    }
}

static const char *hhg_code_gen_replace_ext(
    hhg_code_gen_t *gen,
    const char *in_filename
)
{
    const char *in_basename = fs_basename(in_filename); // "file.hhg"
    const char *in_ext = fs_extention(in_basename); // "hhg"
    if (in_ext == NULL)
        hhg_fatal_error("input file has no extension: %s", in_filename);

    ptrdiff_t in_basename_ext_len = in_ext - in_basename - 1; // len of "file"
    size_t out_filename_ext_len =
        in_basename_ext_len + strlen(gen->backend->ext)
        + 1 /* . */ + 1 /* \0 */; // len of "file.cpp"

    char *out_filename_ext =
        hhg_arena_malloc(gen->arena, out_filename_ext_len); // "file.cpp"

    // check before cast to int in sprintf
    if (in_basename_ext_len > INT_MAX)
        hhg_fatal_error("input filename is too long: %s", in_filename);

    int snprintf_result =
        snprintf(
            out_filename_ext,
            out_filename_ext_len,
            "%.*s.%s",
            (int)in_basename_ext_len,
            in_basename,
            gen->backend->ext
        );

    if (snprintf_result >= out_filename_ext_len)
        hhg_compiler_error(
            "snprintf output truncated: %.*s.%s",
            (int)in_basename_ext_len,
            in_basename,
            gen->backend->ext
        );

    // "out/file.cpp" or "out\file.cpp"
    char *out_filename = hhg_arena_malloc(gen->arena, LIBFS_MAX_PATH);

    int join_path_result = fs_join_path(
        out_filename,
        LIBFS_MAX_PATH,
        gen->out_dir,
        out_filename_ext
    );
    
    if (join_path_result >= LIBFS_MAX_PATH)
        hhg_fatal_error(
            "output path is too long: %s %s",
            gen->out_dir,
            out_filename_ext
        );
    return out_filename;
}
