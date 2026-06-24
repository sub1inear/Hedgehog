#include <string.h>
#include <ctype.h>

#include "mem.h"

#define TOML_API static
#define TOML_IMPLEMENTATION
#define TOML_MALLOC hhg_malloc
#define TOML_REALLOC hhg_realloc
#define TOML_FREE hhg_free
#include <toml.h>
#include <stb_ds.h>
#include <fs.h>

#include "cfg.h"
#include "msg.h"
#include "main.h"
#include "utils.h"

#define hhg_cfg_basic_msg(cfg, type, ...) \
    hhg_basic_msg(cfg->msg_ctx, type, __VA_ARGS__)

#define hhg_cfg_basic_error(cfg, ...) \
    hhg_cfg_basic_msg(cfg, HHG_MSG_ERROR, __VA_ARGS__)
#define hhg_cfg_basic_warning(cfg, ...) \
    hhg_cfg_basic_msg(cfg, HHG_MSG_WARNING, __VA_ARGS__)
#define hhg_cfg_basic_info(cfg, ...) \
    hhg_cfg_basic_msg(cfg, HHG_MSG_INFO, __VA_ARGS__)


// sets variable at offset in cfg to value, with type type
#define hhg_cfg_set_var(cfg, type, offset, value) \
    *(type *)((char *)cfg + offset) = value

typedef struct hhg_cfg_enum_type_data {
    const char *str;
    hhg_cfg_enum_type_t value;
} hhg_cfg_enum_type_data_t;

// supported types:
// TOML_STRING
// TOML_BOOLEAN
// TOML_INT64
// TOML_ARRAY (of strings)
typedef struct hhg_cfg_file_data {
    const char *str;
    size_t offset;
    toml_type_t type;
    hhg_cfg_enum_type_t (*parse_func)(const char *);
} hhg_cfg_file_data_t;

typedef struct hhg_cfg_file {
    const char *filename;
    const hhg_cfg_file_data_t *data;
    size_t data_len;
    FILE *(*find)(hhg_cfg_t *cfg, const char *filename);
} hhg_cfg_file_t;

static bool hhg_cfg_parse_file(hhg_cfg_t *cfg, const hhg_cfg_file_t *file);
static bool hhg_cfg_match(
    hhg_cfg_t *cfg,
    toml_datum_t *tab,
    toml_type_t type,
    toml_datum_t *out,
    const char *key
);
static hhg_cfg_enum_type_t hhg_cfg_parse_enum(
    const char *str,
    const hhg_cfg_enum_type_data_t *data,
    size_t data_size,
    const char *enum_name,
    hhg_cfg_enum_type_t unknown
);
// note: these functions may return NULL
static FILE *hhg_cfg_find_cfg_file(hhg_cfg_t *cfg, const char *filename);
static FILE *hhg_cfg_find_toolchain_file(hhg_cfg_t *cfg, const char *filename);

static const hhg_cfg_file_data_t cfg_file_data[] = {
    { "project.name",                offsetof(hhg_cfg_t, project.name),                    TOML_STRING,  NULL,                         },
    { "project.version",             offsetof(hhg_cfg_t, project.version),                 TOML_STRING,  NULL,                         },
    { "project.std",                 offsetof(hhg_cfg_t, project.std),                     TOML_STRING,  NULL,                         },
    { "global.color",                offsetof(hhg_cfg_t, global.color),                    TOML_BOOLEAN, NULL,                         },
    { "build.entry",                 offsetof(hhg_cfg_t, build.entry),                     TOML_STRING,  NULL,                         },
    { "build.out-dir",               offsetof(hhg_cfg_t, build.out_dir),                   TOML_STRING,  NULL,                         },
    { "build.mode",                  offsetof(hhg_cfg_t, build.mode),                      TOML_STRING,  hhg_cfg_parse_build_mode,     },
    { "build.stage",                 offsetof(hhg_cfg_t, build.stage),                     TOML_STRING,  hhg_cfg_parse_build_stage,    },
    { "build.debug-stage",           offsetof(hhg_cfg_t, build.debug_stage),               TOML_STRING,  hhg_cfg_parse_build_stage,    },
    { "build.target",                offsetof(hhg_cfg_t, build.target),                    TOML_STRING,  NULL,                         },
    { "build.backend",               offsetof(hhg_cfg_t, build.backend),                   TOML_STRING,  hhg_cfg_parse_backend,        },
    { "build.incremental",           offsetof(hhg_cfg_t, build.incremental),               TOML_BOOLEAN, NULL,                         },
    { "build.warnings",              offsetof(hhg_cfg_t, build.warnings),                  TOML_STRING,  hhg_cfg_parse_build_warnings, },
    { "build.error-warnings",        offsetof(hhg_cfg_t, build.error_warnings),            TOML_BOOLEAN, NULL,                         },
    { "run.args",                    offsetof(hhg_cfg_t, run.args),                        TOML_ARRAY,   NULL,                         },
    { "test.test-dir",               offsetof(hhg_cfg_t, test.test_dir),                   TOML_STRING,  NULL,                         },
    { "test.list",                   offsetof(hhg_cfg_t, test.list),                       TOML_BOOLEAN, NULL,                         },
    { "test.fail-fast",              offsetof(hhg_cfg_t, test.fail_fast),                  TOML_BOOLEAN, NULL,                         },
    { "test.threads",                offsetof(hhg_cfg_t, test.threads),                    TOML_INT64,   NULL,                         },
    { "test.filter",                 offsetof(hhg_cfg_t, test.filter),                     TOML_STRING,  NULL,                         },
    { "clean.force",                 offsetof(hhg_cfg_t, clean.force),                     TOML_BOOLEAN, NULL,                         },
    { "clean.dry-run",               offsetof(hhg_cfg_t, clean.dry_run),                   TOML_BOOLEAN, NULL,                         },
    { "repl.tmp-dir",                offsetof(hhg_cfg_t, repl.tmp_dir),                    TOML_STRING,  NULL,                         },
    { "repl.target",                 offsetof(hhg_cfg_t, repl.target),                     TOML_STRING,  NULL,                         },
    { "repl.backend",                offsetof(hhg_cfg_t, repl.backend),                    TOML_STRING,  hhg_cfg_parse_backend,        },
};

static const hhg_cfg_file_data_t toolchain_file_data[] = {
    { "cpp.compiler.cmd",            offsetof(hhg_cfg_t, toolchain.cpp.compiler.cmd),      TOML_STRING, NULL,                          },
    { "cpp.compiler.flags",          offsetof(hhg_cfg_t, toolchain.cpp.compiler.flags),    TOML_ARRAY,  NULL,                          },
    { "cpp.compiler.debug.flags",    offsetof(hhg_cfg_t, toolchain.cpp.compiler.debug),    TOML_ARRAY,  NULL,                          },
    { "cpp.compiler.release.flags",  offsetof(hhg_cfg_t, toolchain.cpp.compiler.release),  TOML_ARRAY,  NULL,                          },
    { "cpp.linker.cmd",              offsetof(hhg_cfg_t, toolchain.cpp.linker.cmd),        TOML_STRING, NULL,                          },
    { "cpp.linker.flags",            offsetof(hhg_cfg_t, toolchain.cpp.linker.flags),      TOML_ARRAY,  NULL,                          },
    { "cpp.linker.debug.flags",      offsetof(hhg_cfg_t, toolchain.cpp.linker.debug),      TOML_ARRAY,  NULL,                          },
    { "cpp.linker.release.flags",    offsetof(hhg_cfg_t, toolchain.cpp.linker.release),    TOML_ARRAY,  NULL,                          },
    { "qbe.compiler.cmd",            offsetof(hhg_cfg_t, toolchain.qbe.compiler.cmd),      TOML_STRING, NULL,                          },
    { "qbe.compiler.flags",          offsetof(hhg_cfg_t, toolchain.qbe.compiler.flags),    TOML_ARRAY,  NULL,                          },
    { "qbe.compiler.debug.flags",    offsetof(hhg_cfg_t, toolchain.qbe.compiler.debug),    TOML_ARRAY,  NULL,                          },
    { "qbe.compiler.release.flags",  offsetof(hhg_cfg_t, toolchain.qbe.compiler.release),  TOML_ARRAY,  NULL,                          },
    { "qbe.assembler.cmd",           offsetof(hhg_cfg_t, toolchain.qbe.assembler.cmd),     TOML_STRING, NULL,                          },
    { "qbe.assembler.flags",         offsetof(hhg_cfg_t, toolchain.qbe.assembler.flags),   TOML_ARRAY,  NULL,                          },
    { "qbe.assembler.debug.flags",   offsetof(hhg_cfg_t, toolchain.qbe.assembler.debug),   TOML_ARRAY,  NULL,                          },
    { "qbe.assembler.release.flags", offsetof(hhg_cfg_t, toolchain.qbe.assembler.release), TOML_ARRAY,  NULL,                          },
    { "qbe.linker.cmd",              offsetof(hhg_cfg_t, toolchain.qbe.linker.cmd),        TOML_STRING, NULL,                          },
    { "qbe.linker.flags",            offsetof(hhg_cfg_t, toolchain.qbe.linker.flags),      TOML_ARRAY,  NULL,                          },
    { "qbe.linker.debug.flags",      offsetof(hhg_cfg_t, toolchain.qbe.linker.debug),      TOML_ARRAY,  NULL,                          },
    { "qbe.linker.release.flags",    offsetof(hhg_cfg_t, toolchain.qbe.linker.release),    TOML_ARRAY,  NULL,                          },
};

static const hhg_cfg_file_t cfg_files[] = {
    { HHG_CFG_FILENAME,       cfg_file_data,       HHG_ARR_SIZE(cfg_file_data),       hhg_cfg_find_cfg_file,      },
    { HHG_TOOLCHAIN_FILENAME, toolchain_file_data, HHG_ARR_SIZE(toolchain_file_data), hhg_cfg_find_toolchain_file },
};

static const char toolchain_txt[] =
    "[cpp]"                    "\n"
                               "\n"
    "[cpp.compiler]"           "\n"
    "cmd = \"g++\""            "\n"
    "flags = []"               "\n"
                               "\n"
    "[cpp.compiler.debug]"     "\n"
    "flags = []"               "\n"
                               "\n"
    "[cpp.compiler.release]"   "\n"
    "flags = []"               "\n"
                               "\n"
    "[cpp.linker]"             "\n"
    "cmd = \"g++\""            "\n"
    "flags = []"               "\n"
                               "\n"
    "[cpp.linker.debug]"       "\n"
    "flags = []"               "\n"
                               "\n"
    "[cpp.linker.release]"     "\n"
    "flags = []"               "\n"
                               "\n"
    "[qbe]"                    "\n"
                               "\n"
    "[qbe.compiler]"           "\n"
    "cmd = \"qbe\""            "\n"
    "flags = []"               "\n"
                               "\n"
    "[qbe.compiler.debug]"     "\n"
    "flags = []"               "\n"
                               "\n"
    "[qbe.compiler.release]"   "\n"
    "flags = []"               "\n"
                               "\n"
    "[qbe.assembler]"          "\n"
    "cmd = \"g++\""            "\n"
    "flags = []"               "\n"
                               "\n"
    "[qbe.assembler.debug]"    "\n"
    "flags = []"               "\n"
                               "\n"
    "[qbe.assembler.release]"  "\n"
    "flags = []"               "\n"
                               "\n"
    "[qbe.linker]"             "\n"
    "cmd = \"g++\""            "\n"
    "flags = []"               "\n"
                               "\n"
    "[qbe.linker.debug]"       "\n"
    "flags = []"               "\n"
                               "\n"
    "[qbe.linker.release]"     "\n"
    "flags = []"               "\n";

void hhg_cfg_init(hhg_cfg_t *cfg, hhg_msg_ctx_t *msg_ctx, hhg_arena_t *arena)
{
    *cfg = (hhg_cfg_t) {
        .project = (hhg_cfg_project_t) {
            .name = "unknown",
            .version = "0.1.0",
            .std = HHG_VERSION,
        },
        .global = (hhg_cfg_global_t) {
            .color = false,
        },
        .init = (hhg_cfg_init_t) {
            // NULL tells hhg_init to use current working directory name
            .name = NULL,
            .version = "0.1.0",
            .std = HHG_VERSION,
        },
        .build = (hhg_cfg_build_t) {
            .entry = "main.hhg",
            .out_dir = "out",
            .mode = HHG_CFG_BUILD_MODE_DEBUG,
            .stage = HHG_CFG_BUILD_STAGE_NONE,
            .debug_stage = HHG_CFG_BUILD_STAGE_NONE,
            .target = "auto",
            .backend = HHG_CFG_BACKEND_CPP,
            .incremental = false,
            .warnings = HHG_CFG_BUILD_WARNINGS_DEFAULT,
            .error_warnings = false,
        },
        .run = (hhg_cfg_run_t) {
            .args = NULL,
        },
        .test = (hhg_cfg_test_t) {
            .test_dir = "test",
            .list = false,
            .fail_fast = false,
            .threads = -1,
            .filter = "",
        },
        .clean = (hhg_cfg_clean_t) {
            .force = false,
            .dry_run = false,
        },
        .repl = (hhg_cfg_repl_t) {
            .tmp_dir = "tmp",
            .target = "auto",
            .backend = HHG_CFG_BACKEND_CPP,
        },
        .toolchain = (hhg_cfg_toolchain_t) {
            .cpp = (hhg_cfg_toolchain_cpp_t) {
                .compiler = (hhg_cfg_toolchain_tool_t) {
                    .cmd = "g++",
                    .flags = NULL,
                    .debug = (hhg_cfg_toolchain_tool_mode_t) {
                        .flags = NULL,
                    },
                    .release = (hhg_cfg_toolchain_tool_mode_t) {
                        .flags = NULL,
                    },
                },
                .linker = (hhg_cfg_toolchain_tool_t) {
                    .cmd = "g++",
                    .flags = NULL,
                    .debug = (hhg_cfg_toolchain_tool_mode_t) {
                        .flags = NULL,
                    },
                    .release = (hhg_cfg_toolchain_tool_mode_t) {
                        .flags = NULL,
                    },
                },
            },
            .qbe = (hhg_cfg_toolchain_qbe_t) {
                .compiler = {
                    .cmd = "qbe",
                    .flags = NULL,
                    .debug = (hhg_cfg_toolchain_tool_mode_t) {
                        .flags = NULL,
                    },
                    .release = (hhg_cfg_toolchain_tool_mode_t) {
                        .flags = NULL,
                    },
                },
                .assembler = (hhg_cfg_toolchain_tool_t) {
                    .cmd = "g++",
                    .flags = NULL,
                    .debug = (hhg_cfg_toolchain_tool_mode_t) {
                        .flags = NULL,
                    },
                    .release = (hhg_cfg_toolchain_tool_mode_t) {
                        .flags = NULL,
                    },
                },
                .linker = {
                    .cmd = "g++",
                    .flags = NULL,
                    .debug = (hhg_cfg_toolchain_tool_mode_t) {
                        .flags = NULL,
                    },
                    .release = (hhg_cfg_toolchain_tool_mode_t) {
                        .flags = NULL,
                    },
                },
            },
        },
        .msg_ctx = msg_ctx,
        .arena = arena,
        .subcmd = HHG_CMD_ARGS_SUBCMD_NONE,
    };
}

bool hhg_cfg_parse(hhg_cfg_t *cfg)
{
    bool result = false;
    for (size_t i = 0; i < HHG_ARR_SIZE(cfg_files); i++) {
        const hhg_cfg_file_t *file = &cfg_files[i];
        if (hhg_cfg_parse_file(cfg, file))
            result = true;
    }
    return result;
}

void hhg_cfg_del(hhg_cfg_t *cfg)
{
    // run is set to NULL so safe to call
    arrfree(cfg->run.args);
}

hhg_cfg_build_mode_t hhg_cfg_parse_build_mode(const char *str)
{
    static const hhg_cfg_enum_type_data_t build_mode_data[] = {
        { "debug",   HHG_CFG_BUILD_MODE_DEBUG   },
        { "release", HHG_CFG_BUILD_MODE_RELEASE },
    };
    return hhg_cfg_parse_enum(
        str,
        build_mode_data,
        HHG_ARR_SIZE(build_mode_data),
        "build mode",
        HHG_CFG_BUILD_MODE_UNKNOWN
    );
}

hhg_cfg_build_stage_t hhg_cfg_parse_build_stage(const char *str)
{
    static const hhg_cfg_enum_type_data_t build_stage_data[] = {
        { "none",      HHG_CFG_BUILD_STAGE_NONE      },
        { "lexer",     HHG_CFG_BUILD_STAGE_LEXER     },
        { "parser",    HHG_CFG_BUILD_STAGE_PARSER    },
        { "sem-an",    HHG_CFG_BUILD_STAGE_SEM_AN    },
        { "mir-gen",   HHG_CFG_BUILD_STAGE_MIR_GEN   },
        { "code-gen",  HHG_CFG_BUILD_STAGE_CODE_GEN  },
        { "ext-build", HHG_CFG_BUILD_STAGE_EXT_BUILD },
    };
    return hhg_cfg_parse_enum(
        str,
        build_stage_data,
        HHG_ARR_SIZE(build_stage_data),
        "build stage",
        HHG_CFG_BUILD_STAGE_UNKNOWN
    );
}

hhg_cfg_build_warnings_t hhg_cfg_parse_build_warnings(const char *str)
{
    static const hhg_cfg_enum_type_data_t build_warnings_data[] = {
        { "none",     HHG_CFG_BUILD_WARNINGS_NONE     },
        { "default",  HHG_CFG_BUILD_WARNINGS_DEFAULT  },
        { "all",      HHG_CFG_BUILD_WARNINGS_ALL      },
        { "pedantic", HHG_CFG_BUILD_WARNINGS_PEDANTIC },
    };
    return hhg_cfg_parse_enum(
        str,
        build_warnings_data,
        HHG_ARR_SIZE(build_warnings_data),
        "build warnings",
        HHG_CFG_BUILD_WARNINGS_UNKNOWN
    );
}

hhg_cfg_backend_t hhg_cfg_parse_backend(const char *str)
{
    static const hhg_cfg_enum_type_data_t backend_data[] = {
        { "cpp", HHG_CFG_BACKEND_CPP },
        { "qbe", HHG_CFG_BACKEND_QBE },
    };
    return hhg_cfg_parse_enum(
        str,
        backend_data,
        HHG_ARR_SIZE(backend_data),
        "backend",
        HHG_CFG_BACKEND_UNKNOWN
    );
}

static bool hhg_cfg_parse_file(hhg_cfg_t *cfg, const hhg_cfg_file_t *cfg_file)
{
    FILE *file = cfg_file->find(cfg, cfg_file->filename);
    if (file == NULL) {
        hhg_cfg_basic_error(
            cfg,
            "failed to open config file `%s`: %s",
            cfg_file->filename,
            strerror(errno)
        );
        return false;
    }
    
    toml_result_t toml_result = toml_parse_file(file);
    fclose(file);

    if (!toml_result.ok) {
        hhg_cfg_basic_error(
            cfg,
            "failed to parse config file `%s`: %s",
            cfg_file->filename,
            toml_result.errmsg
        );
        return false;
    }

    for (size_t i = 0; i < cfg_file->data_len; i++) {
        const hhg_cfg_file_data_t *data = &cfg_file->data[i];
        toml_datum_t value;
        bool match_result = hhg_cfg_match(
            cfg,
            &toml_result.toptab,
            data->type,
            &value,
            data->str
        );
        if (match_result)
            continue;

        if (data->type == TOML_STRING) {
            if (data->parse_func)
                hhg_cfg_set_var(cfg,
                    int,
                    data->offset,
                    data->parse_func(value.u.s)
                );
             else {
                // owned by toml_result_t so need to copy to arena
                const char *str = hhg_arena_strdup(cfg->arena, value.u.s);
                hhg_cfg_set_var(cfg, const char *, data->offset, str);
             }
        } else if (data->type == TOML_BOOLEAN)
            hhg_cfg_set_var(cfg, bool, data->offset, value.u.boolean);
        else if (data->type == TOML_INT64)
            hhg_cfg_set_var(cfg, int64_t, data->offset, value.u.int64);
        else if (data->type == TOML_ARRAY) {
            // convert to stb_ds array and check that all elements are strings
            char **arr = NULL;
            arrsetlen(arr, value.u.arr.size);
            for (int32_t j = 0; j < value.u.arr.size; j++) {
                toml_datum_t *elem = &value.u.arr.elem[j];
                if (elem->type != TOML_STRING)
                    hhg_cfg_basic_error(
                        cfg,
                        "config: expected array of strings for key `%s`, got element type `%s`",
                        data->str,
                        toml_type_to_str(elem->type)
                    );
                // owned by toml_result_t so need to copy to arena
                arr[j] = hhg_arena_strdup(cfg->arena, elem->u.s);
            }
            hhg_cfg_set_var(cfg, char **, data->offset, arr);
        }
    }
    
    toml_free(toml_result);
    return cfg->msg_ctx->error_count > 0;
}

static bool hhg_cfg_match(
    hhg_cfg_t *cfg,
    toml_datum_t *tab,
    toml_type_t type,
    toml_datum_t *out,
    const char *key
)
{
    toml_datum_t datum = toml_seek(*tab, key);
    if (datum.type == TOML_UNKNOWN) {
        hhg_cfg_basic_error(cfg, "config: missing key `%s`", key);
        return true;
    }
    if (datum.type != type) {
        hhg_cfg_basic_error(
            cfg,
            "config: expected type `%s` for key `%s`, got type `%s`",
            toml_type_to_str(type),
            key,
            toml_type_to_str(datum.type)
        );
        return true;
    }
    *out = datum;
    return false;
}

static hhg_cfg_enum_type_t hhg_cfg_parse_enum(
    const char *str,
    const hhg_cfg_enum_type_data_t *data,
    size_t data_size,
    const char *enum_name,
    hhg_cfg_enum_type_t unknown
)
{
    for (size_t i = 0; i < data_size; i++) {
        if (strcmp(str, data[i].str) == 0)
            return data[i].value;
    }
    hhg_fatal_error("invalid %s: %s", enum_name, str);
    return unknown;
}

static FILE *hhg_cfg_find_cfg_file(hhg_cfg_t *cfg, const char *filename)
{
    HHG_UNUSED(cfg);
    // caller will handle failure
    return fopen(filename, "r");
}

static FILE *hhg_cfg_find_toolchain_file(hhg_cfg_t *cfg, const char *filename)
{
    // get executable path
    char exec_path[FS_MAX_PATH];
    if (!fs_exec_path(exec_path))
        hhg_fatal_error("failed to get executable path");

    // remove executable name in-place to get executable dir
    char *rsplit = fs_rsplit(exec_path);
    if (rsplit == NULL)
        exec_path[0] = '\0';
    else
        *rsplit = '\0';

    // merge executable dir with filename to get full config file path
    char filename_path[FS_MAX_PATH];
    hhg_utils_join_path(
        filename_path,
        HHG_ARR_SIZE(filename_path),
        exec_path,
        filename
    );

    FILE *file = fopen(filename_path, "r");
    if (file == NULL) {
        file = fopen(filename_path, "w+");
        if (file == NULL)
            return NULL; // caller will handle failure
        fputs(toolchain_txt, file);
        fseek(file, 0, SEEK_SET);
        hhg_cfg_basic_info(
            cfg,
            "automatically created toolchain file `%s`\n"
            "edit as needed for your machine",
            filename_path
        );
    }

    return file;
}