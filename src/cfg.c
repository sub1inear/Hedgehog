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
#include "utils.h"

// sets variable at offset in cfg to value, with type type
#define hhg_cfg_set_var(cfg, type, offset, value) \
    *(type *)((char *)cfg + offset) = value

static toml_datum_t hhg_cfg_match(
    toml_datum_t *tab,
    const char *key,
    toml_type_t type
);

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
};

void hhg_cfg_init(hhg_cfg_t *cfg, hhg_arena_t *arena) {
    // side effect: sets all fields to 0/NULL so
    // that hhg_cfg_del can safely free them even if
    // hhg_cfg_parse has not been called
    *cfg = (hhg_cfg_t) {
        .arena = arena,
    };

}

void hhg_cfg_parse(hhg_cfg_t *cfg, const char *filename) {
    FILE *file = hhg_utils_fopen(filename, "r");
    
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
                    hhg_fatal_error(
                        "config: expected array of strings for key `%s`",
                        data->str
                    );
                arr[j] = hhg_arena_strdup(cfg->arena, elem->u.s);
            }
            hhg_cfg_set_var(cfg, char **, data->offset, arr);
        }
    }
    
    toml_free(result);

    fclose(file);
}

void hhg_cfg_del(hhg_cfg_t *cfg) {
    // safe even if run has not been set because
    // hhg_cfg_init with compound literal sets all fields to 0/NULL
    arrfree(cfg->run.args);
}

hhg_cfg_build_mode_t hhg_cfg_parse_build_mode(const char *str)
{
    if (strcmp(str, "debug") == 0)
        return HHG_CFG_BUILD_MODE_DEBUG;
    if (strcmp(str, "release") == 0)
        return HHG_CFG_BUILD_MODE_RELEASE;
    hhg_fatal_error("invalid build mode: %s", str);
    return HHG_CFG_BUILD_MODE_UNKNOWN;
}

hhg_cfg_build_stage_t hhg_cfg_parse_build_stage(const char *str)
{
    if (strcmp(str, "none") == 0)
        return HHG_CFG_BUILD_STAGE_NONE;
    if (strcmp(str, "lexer") == 0)
        return HHG_CFG_BUILD_STAGE_LEXER;
    if (strcmp(str, "parser") == 0)
        return HHG_CFG_BUILD_STAGE_PARSER;
    if (strcmp(str, "sem-an") == 0)
        return HHG_CFG_BUILD_STAGE_SEM_AN;
    if (strcmp(str, "mir-gen") == 0)
        return HHG_CFG_BUILD_STAGE_MIR_GEN;
    if (strcmp(str, "ext-build") == 0)
        return HHG_CFG_BUILD_STAGE_EXT_BUILD;
    hhg_fatal_error("invalid build stage: %s", str);
    return HHG_CFG_BUILD_MODE_UNKNOWN;
}

hhg_cfg_build_warnings_t hhg_cfg_parse_build_warnings(const char *str)
{
    if (strcmp(str, "none") == 0)
        return HHG_CFG_BUILD_WARNINGS_NONE;
    if (strcmp(str, "default") == 0)
        return HHG_CFG_BUILD_WARNINGS_DEFAULT;
    if (strcmp(str, "all") == 0)
        return HHG_CFG_BUILD_WARNINGS_ALL;
    if (strcmp(str, "pedantic") == 0)
        return HHG_CFG_BUILD_WARNINGS_PEDANTIC;
    hhg_fatal_error("invalid build warnings: %s", str);
    return HHG_CFG_BUILD_MODE_UNKNOWN;
}

hhg_cfg_build_backend_t hhg_cfg_parse_build_backend(const char *str)
{
    if (strcmp(str, "cpp") == 0)
        return HHG_CFG_BUILD_BACKEND_CPP;
    if (strcmp(str, "qbe") == 0)
        return HHG_CFG_BUILD_BACKEND_QBE;
    hhg_fatal_error("invalid code gen backend: %s", str);
    return HHG_CFG_BUILD_BACKEND_UNKNOWN;
}

hhg_cfg_clean_mode_t hhg_cfg_parse_clean_mode(const char *str)
{
    if (strcmp(str, "all") == 0)
        return HHG_CFG_CLEAN_MODE_ALL;
    if (strcmp(str, "build") == 0)
        return HHG_CFG_CLEAN_MODE_BUILD;
    if (strcmp(str, "gen") == 0)
        return HHG_CFG_CLEAN_MODE_GEN;
    hhg_fatal_error("invalid clean mode: %s", str);
    return HHG_CFG_CLEAN_MODE_UNKNOWN;
}

static toml_datum_t hhg_cfg_match(
    toml_datum_t *tab,
    const char *key,
    toml_type_t type
)
{
    toml_datum_t datum = toml_seek(*tab, key);
    if (datum.type == TOML_UNKNOWN)
        hhg_fatal_error("config: missing key `%s`", key);
    if (datum.type != type)
        hhg_fatal_error(
            "config: expected type %d for key `%s`, got %d",
            type,
            key,
            toml_type_to_str(datum.type)
        );
    return datum;
}
