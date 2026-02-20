#include <string.h>

#include "mem.h"

#define TOML_API static
#define TOML_IMPLEMENTATION
#define TOML_MALLOC hhg_malloc
#define TOML_REALLOC hhg_realloc
#define TOML_FREE hhg_free
#include <toml.h>
#include <stb_ds.h>

#include "cfg.h"
#include "msg.h"
#include "main.h"
#include "utils.h"

#define hhg_cfg_basic_msg(cfg, type, msg, ...) \
    hhg_basic_msg(&cfg->msg_ctx, type, msg, __VA_ARGS__)

#define hhg_cfg_basic_error(cfg, msg, ...) \
    hhg_cfg_basic_msg(cfg, HHG_MSG_ERROR, msg, __VA_ARGS__)
#define hhg_cfg_basic_warning(cfg, msg, ...) \
    hhg_cfg_basic_msg(cfg, HHG_MSG_WARNING, msg, __VA_ARGS__)
#define hhg_cfg_basic_info(cfg, msg, ...) \
    hhg_cfg_basic_msg(cfg, HHG_MSG_INFO, msg, __VA_ARGS__)


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
typedef struct hhg_cfg_data {
    const char *str;
    size_t offset;
    toml_type_t type;
    hhg_cfg_enum_type_t (*parse_func)(const char *);
} hhg_cfg_data_t;

static const hhg_cfg_data_t cfg_data[] = {
    { "project.name",         offsetof(hhg_cfg_t, project.name),         TOML_STRING,  NULL },
    { "project.version",      offsetof(hhg_cfg_t, project.version),      TOML_STRING,  NULL },
    { "project.std",          offsetof(hhg_cfg_t, project.std),          TOML_STRING,  NULL },
    { "global.color",         offsetof(hhg_cfg_t, global.color),         TOML_BOOLEAN, NULL },
    { "build.entry",          offsetof(hhg_cfg_t, build.entry),          TOML_STRING,  NULL },
    { "build.out-dir",        offsetof(hhg_cfg_t, build.out_dir),        TOML_STRING,  NULL },
    { "build.mode",           offsetof(hhg_cfg_t, build.mode),           TOML_STRING,  hhg_cfg_parse_build_mode },
    { "build.stage",          offsetof(hhg_cfg_t, build.stage),          TOML_STRING,  hhg_cfg_parse_build_stage },
    { "build.debug-stage",    offsetof(hhg_cfg_t, build.debug_stage),    TOML_STRING,  hhg_cfg_parse_build_stage },
    { "build.target",         offsetof(hhg_cfg_t, build.target),         TOML_STRING,  NULL },
    { "build.backend",        offsetof(hhg_cfg_t, build.backend),        TOML_STRING,  hhg_cfg_parse_backend },
    { "build.incremental",    offsetof(hhg_cfg_t, build.incremental),    TOML_BOOLEAN, NULL },
    { "build.warnings",       offsetof(hhg_cfg_t, build.warnings),       TOML_STRING,  hhg_cfg_parse_build_warnings },
    { "build.error-warnings", offsetof(hhg_cfg_t, build.error_warnings), TOML_BOOLEAN, NULL },
    { "run.args",             offsetof(hhg_cfg_t, run.args),             TOML_ARRAY,   NULL },
    { "test.test-dir",        offsetof(hhg_cfg_t, test.test_dir),        TOML_STRING,  NULL },
    { "test.list",            offsetof(hhg_cfg_t, test.list),            TOML_BOOLEAN, NULL },
    { "test.fail-fast",       offsetof(hhg_cfg_t, test.fail_fast),       TOML_BOOLEAN, NULL },
    { "test.threads",         offsetof(hhg_cfg_t, test.threads),         TOML_INT64,   NULL },
    { "test.filter",          offsetof(hhg_cfg_t, test.filter),          TOML_STRING,  NULL },
    { "clean.mode",           offsetof(hhg_cfg_t, clean.mode),           TOML_STRING,  hhg_cfg_parse_clean_mode },
    { "clean.force",          offsetof(hhg_cfg_t, clean.force),          TOML_BOOLEAN, NULL },
    { "clean.dry-run",        offsetof(hhg_cfg_t, clean.dry_run),        TOML_BOOLEAN, NULL },
    { "repl.tmp-dir",         offsetof(hhg_cfg_t, repl.tmp_dir),         TOML_STRING,  NULL },
    { "repl.target",          offsetof(hhg_cfg_t, repl.target),          TOML_STRING,  NULL },
    { "repl.backend",         offsetof(hhg_cfg_t, repl.backend),         TOML_STRING,  hhg_cfg_parse_backend },
};

static toml_datum_t hhg_cfg_match(
    hhg_cfg_t *cfg,
    toml_datum_t *tab,
    const char *key,
    toml_type_t type
);
static hhg_cfg_enum_type_t hhg_cfg_parse_enum(
    const char *str,
    const hhg_cfg_enum_type_data_t *data,
    size_t data_size,
    const char *enum_name,
    hhg_cfg_enum_type_t unknown
);

void hhg_cfg_init(hhg_cfg_t *cfg, hhg_arena_t *arena)
{
    *cfg = (hhg_cfg_t) {
        .global = (hhg_cfg_global_t) {
            .color = false,
        },
        .init = (hhg_cfg_init_t) {
            .name = "unknown",
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
            .mode = HHG_CFG_CLEAN_MODE_ALL,
            .force = false,
            .dry_run = false,
        },
        .repl = (hhg_cfg_repl_t) {
            .tmp_dir = "tmp",
            .target = "auto",
            .backend = HHG_CFG_BACKEND_CPP,
        },
        .arena = arena,
        .subcmd = HHG_CMD_ARGS_SUBCMD_NONE,
    };

    hhg_msg_ctx_init(&cfg->msg_ctx, cfg);
}

bool hhg_cfg_parse(hhg_cfg_t *cfg, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
        return false;
    
    toml_result_t result = toml_parse_file(file);
    if (!result.ok)
        hhg_fatal_error(
            "failed to parse config file %s: %s",
            filename,
            result.errmsg
        );
    for (size_t i = 0; i < HHG_ARR_SIZE(cfg_data); i++) {
        const hhg_cfg_data_t *data = &cfg_data[i];
        toml_datum_t value = hhg_cfg_match(
            cfg,
            &result.toptab,
            data->str,
            data->type
        );

        if (data->type == TOML_STRING) {
            if (data->parse_func)
                hhg_cfg_set_var(cfg,
                    int,
                    data->offset,
                    data->parse_func(value.u.s)
                );
             else
                hhg_cfg_set_var(cfg, const char *, data->offset, value.u.s);
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
                // don't need to free original string (owned by toml_result_t)
                arr[j] = hhg_arena_strdup(cfg->arena, elem->u.s);
            }
            hhg_cfg_set_var(cfg, char **, data->offset, arr);
        }
    }
    
    toml_free(result);

    fclose(file);
    return cfg->msg_ctx.error_count > 0;
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

hhg_cfg_clean_mode_t hhg_cfg_parse_clean_mode(const char *str)
{
    static const hhg_cfg_enum_type_data_t clean_mode_data[] = {
        { "all",   HHG_CFG_CLEAN_MODE_ALL   },
        { "build", HHG_CFG_CLEAN_MODE_BUILD },
        { "gen",   HHG_CFG_CLEAN_MODE_GEN   },
    };
    return hhg_cfg_parse_enum(
        str,
        clean_mode_data,
        HHG_ARR_SIZE(clean_mode_data),
        "clean mode",
        HHG_CFG_CLEAN_MODE_UNKNOWN
    );

}

static toml_datum_t hhg_cfg_match(
    hhg_cfg_t *cfg,
    toml_datum_t *tab,
    const char *key,
    toml_type_t type
)
{
    toml_datum_t datum = toml_seek(*tab, key);
    if (datum.type == TOML_UNKNOWN)
        hhg_cfg_basic_error(cfg, "config: missing key `%s`", key);
    if (datum.type != type)
        hhg_cfg_basic_error(
            cfg,
            "config: expected type %s for key `%s`, got %s",
            toml_type_to_str(type),
            key,
            toml_type_to_str(datum.type)
        );
    return datum;
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
