#ifndef HHG_CFG_H
#define HHG_CFG_H

#include <stdint.h>
#include <stdbool.h>

#include "build.h"
#include "mem.h" // hhg_arena_t is based off arena_t so can't forward declare
#include "msg.h"

typedef struct hhg_cfg_project {
    char *name;
    char *version;
    char *std;
} hhg_cfg_project_t;

typedef struct hhg_cfg_global {
    bool color;
} hhg_cfg_global_t;

typedef struct hhg_cfg_init {
    const char *name;
    const char *version;
    const char *std;
} hhg_cfg_init_t;

// used so we can have a function pointer in cfg_data
// returning a generic value for parsing strs to enum values
// otherwise, enum return types would be different
// this standarizes the return type to int (which is not guarenteed in C)
typedef int hhg_cfg_enum_type_t;

// *_UNKNOWN is used as a return value for conversion functions

enum hhg_cfg_build_mode {
    HHG_CFG_BUILD_MODE_UNKNOWN,
    HHG_CFG_BUILD_MODE_DEBUG,
    HHG_CFG_BUILD_MODE_RELEASE,
};
typedef hhg_cfg_enum_type_t hhg_cfg_build_mode_t;

// shared between build and repl
enum hhg_cfg_backend {
    HHG_CFG_BACKEND_UNKNOWN,
    HHG_CFG_BACKEND_CPP,
    HHG_CFG_BACKEND_QBE,
};
typedef hhg_cfg_enum_type_t hhg_cfg_backend_t;

enum hhg_cfg_build_stage {
    HHG_CFG_BUILD_STAGE_UNKNOWN,
    HHG_CFG_BUILD_STAGE_NONE,
    HHG_CFG_BUILD_STAGE_LEXER,
    HHG_CFG_BUILD_STAGE_PARSER,
    HHG_CFG_BUILD_STAGE_SEM_AN,
    HHG_CFG_BUILD_STAGE_MIR_GEN,
    HHG_CFG_BUILD_STAGE_CODE_GEN,
    HHG_CFG_BUILD_STAGE_EXT_BUILD
};
typedef hhg_cfg_enum_type_t hhg_cfg_build_stage_t;

enum hhg_cfg_build_warnings {
    HHG_CFG_BUILD_WARNINGS_UNKNOWN,
    HHG_CFG_BUILD_WARNINGS_NONE,
    HHG_CFG_BUILD_WARNINGS_DEFAULT,
    HHG_CFG_BUILD_WARNINGS_ALL,
    HHG_CFG_BUILD_WARNINGS_PEDANTIC,
};
typedef hhg_cfg_enum_type_t hhg_cfg_build_warnings_t;

typedef struct hhg_cfg_build {
    const char *entry;
    const char *out_dir;
    hhg_cfg_build_mode_t mode;
    hhg_cfg_build_stage_t stage;
    hhg_cfg_build_stage_t debug_stage;
    char *target;
    hhg_cfg_backend_t backend;
    bool incremental;
    hhg_cfg_build_warnings_t warnings;
    bool error_warnings;
} hhg_cfg_build_t;

typedef struct hhg_cfg_run {
    char **args;
} hhg_cfg_run_t;

typedef struct hhg_cfg_test {
    const char *test_dir;
    bool list;
    bool fail_fast;
    int64_t threads;
    const char *filter;
} hhg_cfg_test_t;

enum hhg_cfg_clean_mode {
    HHG_CFG_CLEAN_MODE_UNKNOWN,
    HHG_CFG_CLEAN_MODE_ALL,
    HHG_CFG_CLEAN_MODE_BUILD,
    HHG_CFG_CLEAN_MODE_GEN,
};
typedef hhg_cfg_enum_type_t hhg_cfg_clean_mode_t;

typedef struct hhg_cfg_clean {
    hhg_cfg_clean_mode_t mode;
    bool force;
    bool dry_run;
} hhg_cfg_clean_t;

typedef struct hhg_cfg_repl {
    const char *tmp_dir;
    const char *target;
    hhg_cfg_backend_t backend;
} hhg_cfg_repl_t;

typedef struct hhg_cfg {
    hhg_cfg_project_t project;
    hhg_cfg_global_t global;
    hhg_cfg_init_t init;
    hhg_cfg_build_t build;
    hhg_cfg_run_t run;
    hhg_cfg_test_t test;
    hhg_cfg_clean_t clean;
    hhg_cfg_repl_t repl;
    hhg_msg_ctx_t msg_ctx;
    hhg_arena_t *arena;
} hhg_cfg_t;

void hhg_cfg_init(hhg_cfg_t *cfg, hhg_arena_t *arena);

// parses filename into cfg
// returns true if there were parsing errors,
// false otherwise (including file not found)
bool hhg_cfg_parse(hhg_cfg_t *cfg, const char *filename);

void hhg_cfg_del(hhg_cfg_t *cfg);

hhg_cfg_build_mode_t hhg_cfg_parse_build_mode(const char *str);
hhg_cfg_build_stage_t hhg_cfg_parse_build_stage(const char *str);
hhg_cfg_build_warnings_t hhg_cfg_parse_build_warnings(const char *str);
hhg_cfg_backend_t hhg_cfg_parse_backend(const char *str);
hhg_cfg_clean_mode_t hhg_cfg_parse_clean_mode(const char *str);

#endif
