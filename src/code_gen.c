#include <stdio.h>

#include <fs.h>

#include "code_gen.h"
#include "main.h"
#include "mem.h"
#include "mir.h"
#include "msg.h"
#include "str.h"
#include "utils.h"

static const char *hhg_code_gen_file_replace_dir_ext(hhg_code_gen_t *gen,
                                                     const char *filename,
                                                     const char *dir,
                                                     const char *ext);

void hhg_code_gen_init(hhg_code_gen_t *gen, hhg_arena_t *arena)
{
    *gen = (hhg_code_gen_t){
        .arena = arena,
    };
    // create directory if it doesn't exist
    if (!fs_exist(".hhg"))
        if (!fs_make_dir(".hhg"))
            hhg_fatal_error("failed to create output directory: .hhg");
}

bool hhg_code_gen_run(hhg_code_gen_t *code_gen, hhg_mir_gen_t *mir_gen,
                      const char *filename, const char **out_filename)
{
    HHG_UNUSED(mir_gen);
    *out_filename = hhg_code_gen_file_replace_dir_ext(code_gen, filename,
                                                      HHG_TMP_DIR, ".cpp");
    FILE *file = hhg_fopen(*out_filename, "w");
    fputs("#include <stdio.h>\nint main() { printf(\"Hello, world!\\n\"); }\n",
          file);
    fclose(file);
    return true;
}

void hhg_code_gen_print(hhg_code_gen_t *gen)
{
    HHG_UNUSED(gen);
}

void hhg_code_gen_del(hhg_code_gen_t *gen)
{
    HHG_UNUSED(gen);
}

static const char *
hhg_code_gen_file_replace_dir_ext(hhg_code_gen_t *gen,
                                  const char *filename, // path/to/file.hhg
                                  const char *dir,      // .hhg
                                  const char *ext       // .cpp
)
{
    char *basename =
        hhg_arena_strdup(gen->arena, fs_basename(filename)); // file.hhg
    char *basename_ext = fs_extention(basename);             // .hhg
    if (basename_ext == NULL)
        hhg_compiler_error("file has no extension: %s", filename);
    *basename_ext = '\0'; // basename -> file

    hhg_str_t str;
    hhg_str_init_str(&str, basename);
    hhg_str_append_str(&str, ext); // file.cpp

    char *out = hhg_arena_malloc(gen->arena, FS_MAX_PATH);

    hhg_join_path(out, FS_MAX_PATH, dir, str.str);

    hhg_str_del(&str);

    return out; // .hhg/file.cpp
}
