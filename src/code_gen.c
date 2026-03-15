#include <stdio.h>
#include <limits.h>
#include <string.h>

#include <fs.h>

#include "code_gen.h"
#include "cfg.h"
#include "cpp_gen.h"
#include "qbe_gen.h"
#include "mir.h"
#include "mem.h"
#include "msg.h"
#include "utils.h"

hhg_code_gen_backend_t *hhg_code_gen_backend_new(
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

void hhg_code_gen_init(
    hhg_code_gen_t *gen,
    hhg_code_gen_backend_t *backend,
    const char *in_filename,
    const char *out_dir,
    hhg_arena_t *arena
)
{
    // create directory if it doesn't exist
    if (!fs_exist(out_dir))
        if (!fs_make_dir(out_dir))
            hhg_fatal_error("failed to create output directory: %s", out_dir);

    // construct output filename by replacing input file extension with backend extension
    const char *in_basename = fs_basename(in_filename);

    const char *in_ext = fs_extention(in_basename);
    if (in_ext == NULL)
        hhg_fatal_error("input file has no extension: %s", in_filename);

    size_t basename_len = in_ext - in_basename - 1;
    
    size_t out_filename_len =
        basename_len + strlen(backend->ext) + 1 /* . */ + 1 /* \0 */;

    char *out_filename = hhg_arena_malloc(arena, out_filename_len);
    
    // check before cast to int in sprintf
    if (basename_len > INT_MAX)
        hhg_fatal_error("input filename too long: %s", in_filename);
    
    int snprintf_result =
        snprintf(
            out_filename,
            out_filename_len,
            "%.*s.%s",
            (int)basename_len,
            in_basename,
            backend->ext
        );

    if (snprintf_result >= out_filename_len)
        hhg_compiler_error(
            "snprintf output truncated: %.*s.%s",
            (int)basename_len,
            in_basename,
            backend->ext
        );
    
    // join output directory and filename
    char out_path[LIBFS_MAX_PATH];
    
    int join_path_result = fs_join_path(
        out_path,
        LIBFS_MAX_PATH,
        out_dir,
        out_filename
    );
    
    if (join_path_result >= LIBFS_MAX_PATH)
        hhg_fatal_error(
            "output path too long: %s %s",
            out_dir,
            out_filename
        );

    *gen = (hhg_code_gen_t) {
        .backend = backend,
        .file = hhg_utils_fopen(out_path, "w"),
        .arena = arena,
    };
}

void hhg_code_gen_run(hhg_code_gen_t *code_gen, hhg_mir_gen_t *mir_gen)
{
    HHG_UNUSED(code_gen, mir_gen);
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

void hhg_code_gen_backend_free(hhg_code_gen_backend_t *backend)
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

void hhg_code_gen_del(hhg_code_gen_t *gen)
{
    fclose(gen->file);
}
