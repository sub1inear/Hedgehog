#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <stb_ds.h>
#include <fs.h>

#include "ext_build.h"
#include "str.h"
#include "msg.h"
#include "mem.h"
#include "main.h"
#include "utils.h"

#define HHG_EXT_BUILD_CL_ENV_BUFFER_SIZE 4096

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

#ifdef HHG_WINDOWS
static const char **hhg_ext_build_cl_setup(
    hhg_ext_build_t *ext_build,
    const char **argv
);

static const char **hhg_ext_build_cl_copy_include(
    hhg_ext_build_t *ext_build,
    const char **argv,
    const char *cl_env
);

static const char **hhg_ext_build_cl_copy_lib(
    hhg_ext_build_t *ext_build,
    const char **argv,
    const char *cl_env
);

static const char **hhg_ext_build_cl_copy_core(
    hhg_ext_build_t *ext_build,
    const char **argv,
    const char *cl_env,
    const char *key,
    const char **(*copy_func)(
        hhg_ext_build_t *ext_build,
        const char **argv,
        const char *loc,
        ptrdiff_t len   
    )
);
static const char **hhg_ext_build_cl_copy_include_core(
    hhg_ext_build_t *ext_build,
    const char **argv,
    const char *loc,
    ptrdiff_t len
);
static const char **hhg_ext_build_cl_copy_lib_core(
    hhg_ext_build_t *ext_build,
    const char **argv,
    const char *loc,
    ptrdiff_t len
);
#endif

static const hhg_ext_build_cxx_data_t ext_build_cxx_data[] = {
    { "gcc", "-o", "-O2" },
    { "clang", "-o", "-O2" },
    { "cl", "/Fe:", "/O2" },
};

void hhg_ext_build_init(
    hhg_ext_build_t *ext_build,
    hhg_msg_ctx_t *msg_ctx,
    hhg_arena_t *arena
)
{
    *ext_build = (hhg_ext_build_t) {
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

    const char *cxx = getenv("CXX");
    if (cxx == NULL)
        hhg_fatal_error("please specify a C++ compiler with --cxx or the CXX environment variable");
    
    const char *cxx_name =
        hhg_arena_strdup(ext_build->arena, fs_basename(cxx));
    *fs_extention(cxx_name) = '\0';
    
    size_t i;
    for (i = 0; i < HHG_ARR_LEN(ext_build_cxx_data); i++)
        if (strcmp(cxx_name, ext_build_cxx_data[i].cxx) == 0)
            break;

    if (i == HHG_ARR_LEN(ext_build_cxx_data))
        hhg_fatal_error("unsupported C++ compiler: %s", cxx);

    const hhg_ext_build_cxx_data_t *cxx_data = &ext_build_cxx_data[i];

    const char **argv = NULL;
    arrput(argv, cxx);
    arrput(argv, filename);
    arrput(argv, cxx_data->out_flag);

    if (out == NULL)
        out = hhg_file_to_exec(ext_build->arena, orig_filename);
    arrput(argv, out);

    if (release)
        arrput(argv, cxx_data->release_flag);

    if (!strcmp(cxx_name, "cl"))
#ifdef HHG_WINDOWS
        argv = hhg_ext_build_cl_setup(ext_build, argv);
#else
        hhg_fatal_error("`cl` is only supported on Windows");
#endif

    arrput(argv, NULL);


    hhg_str_t stdouterr;
    hhg_str_init(&stdouterr);

    int exit_code = hhg_spawn(argv, &stdouterr);
    
    arrfree(argv);

    if (exit_code != EXIT_SUCCESS)
        hhg_compiler_error(
            "external build failed: `%s` exited with code %i\noutput:\n%s",
            cxx,
            exit_code,
            stdouterr.str
        );
    hhg_str_del(&stdouterr);
}

static const char **hhg_ext_build_cl_setup(
    hhg_ext_build_t *ext_build,
    const char **argv
)
{
    char cl_env_cache_path[FS_MAX_PATH];
    hhg_join_path(
        cl_env_cache_path,
        FS_MAX_PATH,
        HHG_TMP_DIR,
        HHG_CL_ENV_CACHE_FILENAME
    );

    hhg_str_t cl_env;
    hhg_str_init(&cl_env);

    if (fs_exist(cl_env_cache_path)) {
        FILE *cl_env_cache_file = hhg_fopen(cl_env_cache_path, "r");
        char buffer[HHG_EXT_BUILD_CL_ENV_BUFFER_SIZE];
        while (true) {
            size_t len = fread(buffer, 1, sizeof(buffer), cl_env_cache_file);
            if (len != sizeof(buffer))
                break;
            hhg_str_append_str_len(&cl_env, buffer, len);
        }
        if (ferror(cl_env_cache_file))
            hhg_fatal_error(
                "%s: error reading file: %s",
                cl_env_cache_path,
                strerror(errno)
            );
        fclose(cl_env_cache_file);
    } else {
        hhg_ext_build_info(ext_build, "setting up environment for cl.exe");

        // cl.exe requires vsvars64.bat to be run first
        const char *program_files_x86 = getenv("ProgramFiles(x86)");
        if (program_files_x86 == NULL)
            program_files_x86 = "C:\\Program Files (x86)";

        hhg_str_t vswhere_path;
        hhg_str_init_str(&vswhere_path, program_files_x86);
        hhg_str_append_str(
            &vswhere_path,
            "\\Microsoft Visual Studio\\Installer\\vswhere.exe"
        );
    
        hhg_str_t vswhere_stdouterr;
        hhg_str_init(&vswhere_stdouterr);

        int vswhere_exit_code = hhg_spawn(
            (const char *[]){
                vswhere_path.str,
                "-latest",
                "-products",
                "*",
                "-requires",
                "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
                "-property",
                "installationPath",
                NULL
            },
            &vswhere_stdouterr
        );
    
        if (vswhere_exit_code != EXIT_SUCCESS)
            hhg_fatal_error(
                "external build failed: `vswhere.exe` exited with code %i\noutput:\n%s",
                vswhere_exit_code,
                vswhere_stdouterr.str
            );

        while (vswhere_stdouterr.len > 0 &&
               isspace(
                   (unsigned char)vswhere_stdouterr.str[vswhere_stdouterr.len - 1]
               ))
            HHG_UNUSED(hhg_str_pop(&vswhere_stdouterr));
        
        if (vswhere_stdouterr.len == 0)
            hhg_fatal_error(
                "external build failed: `vswhere.exe` did not return a path to Visual Studio"
            );

        hhg_str_t vsvars_path;
        // re-strlen to save memory; popping causes too-large capacity
        hhg_str_init_str(&vsvars_path, vswhere_stdouterr.str);
        
        hhg_str_del(&vswhere_stdouterr);

        hhg_str_append_str(
            &vsvars_path,
            "\\VC\\Auxiliary\\Build\\vcvars64.bat"
        );

        hhg_str_t cmdline;
        hhg_str_init_fmt(&cmdline, "cmd.exe /c \"%s\" && set", vsvars_path.str);
    
        hhg_str_del(&vsvars_path);

        int vcvars_exit_code = hhg_spawn_cmdline(cmdline.str, &cl_env);
        if (vcvars_exit_code != EXIT_SUCCESS)
            hhg_fatal_error(
                "external build failed: `vcvars64.bat` exited with code %i\noutput:\n%s",
                vcvars_exit_code,
                cl_env.str
            );

        FILE *cl_env_cache_file = hhg_fopen(cl_env_cache_path, "w");
        fwrite(cl_env.str, 1, cl_env.len, cl_env_cache_file);
        fclose(cl_env_cache_file);  
    }

    argv = hhg_ext_build_cl_copy_include(ext_build, argv, cl_env.str);
    argv = hhg_ext_build_cl_copy_lib(ext_build, argv, cl_env.str);

    return argv; // array may resize and invalidate pointer
}

static const char **hhg_ext_build_cl_copy_include(
    hhg_ext_build_t *ext_build,
    const char **argv,
    const char *cl_env
)
{
    return hhg_ext_build_cl_copy_core(
        ext_build,
        argv,
        cl_env,
        "INCLUDE",
        hhg_ext_build_cl_copy_include_core
    );

}

static const char **hhg_ext_build_cl_copy_lib(
    hhg_ext_build_t *ext_build,
    const char **argv,
    const char *cl_env
)
{
    arrput(argv, "/link");
    argv = hhg_ext_build_cl_copy_core(
        ext_build,
        argv,
        cl_env,
        "LIB",
        hhg_ext_build_cl_copy_lib_core
    );
    return argv;
}

static const char **hhg_ext_build_cl_copy_core(
    hhg_ext_build_t *ext_build,
    const char **argv,
    const char *cl_env,
    const char *key,
    const char **(*copy_func)(
        hhg_ext_build_t *ext_build,
        const char **argv,
        const char *loc,
        ptrdiff_t len
    )
)
{
    const char *loc = strstr(cl_env, key);
    if (loc == NULL)
        hhg_fatal_error(
            "external build failed: `%s` not found in environment variables",
            key
        );

    loc += strlen(key);
    loc++; // skip '='

    const char *end_of_var = strchr(loc, '\r');

    while (true) {
        const char *end = strchr(loc, ';');

        bool is_last = end == NULL || end > end_of_var;
        if (is_last)
            end = end_of_var;

        ptrdiff_t len = end - loc;
        
        argv = copy_func(ext_build, argv, loc, len);
        
        if (is_last)
            break;
        loc = end + 1;
    }
    return argv;
}

static const char **hhg_ext_build_cl_copy_include_core(
    hhg_ext_build_t *ext_build,
    const char **argv,
    const char *loc,
    ptrdiff_t len
)
{
    char *include_path = hhg_arena_malloc(ext_build->arena, len + 1);
    memcpy(include_path, loc, len);
    include_path[len] = '\0';
        
    arrput(argv, "/I");
    arrput(argv, include_path);
    
    return argv;
}

static const char **hhg_ext_build_cl_copy_lib_core(
    hhg_ext_build_t *ext_build,
    const char **argv,
    const char *loc,
    ptrdiff_t len
)
{
    char *lib_path =
        hhg_arena_malloc(
            ext_build->arena,
            HHG_STR_LEN("/LIBPATH:") + len + 1 /* \0 */
        );
    strcpy(lib_path, "/LIBPATH:");
    memcpy(lib_path + HHG_STR_LEN("/LIBPATH:"), loc, len);
    lib_path[len + HHG_STR_LEN("/LIBPATH:")] = '\0';

    arrput(argv, lib_path);

    return argv;
}
