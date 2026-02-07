#ifndef HHG_CMD_ARGS_H
#define HHG_CMD_ARGS_H

#include <stdint.h>
#include <stdbool.h>
#include "build.h"

typedef enum hhg_cmd_args_subcmd_type {
    HHG_CMD_ARGS_SUBCMD_NONE,
    HHG_CMD_ARGS_SUBCMD_INIT,
    HHG_CMD_ARGS_SUBCMD_BUILD,
    HHG_CMD_ARGS_SUBCMD_RUN,
    HHG_CMD_ARGS_SUBCMD_TEST,
    HHG_CMD_ARGS_SUBCMD_CLEAN,
    HHG_CMD_ARGS_SUBCMD_REPL,
} hhg_cmd_args_subcmd_type_t;

typedef enum hhg_cmd_args_output_target {
    HHG_CMD_ARGS_OUTPUT_TARGET_AUTO,
    HHG_CMD_ARGS_OUTPUT_TARGET_CPP,
    HHG_CMD_ARGS_OUTPUT_TARGET_ASM,
} hhg_cmd_args_output_target_t;

typedef enum hhg_cmd_args_build_warnings {
    HHG_CMD_ARGS_BUILD_WARNINGS_NONE,
    HHG_CMD_ARGS_BUILD_WARNINGS_DEFAULT,
    HHG_CMD_ARGS_BUILD_WARNINGS_ALL,
    HHG_CMD_ARGS_BUILD_WARNINGS_PEDANTIC,
} hhg_cmd_args_build_warnings_t;

typedef struct hhg_cmd_args_subcmd_init {
    char *name;
} hhg_cmd_args_subcmd_init_t;

typedef struct hhg_cmd_args_subcmd_build {
    bool release;
    bool debug;
    hhg_build_stage_t stage;
    hhg_build_stage_t debug_stage;
    char *target;
    char *output_target;
    hhg_cmd_args_build_warnings_t warnings;
    bool deny_warnings;
} hhg_cmd_args_subcmd_build_t;

typedef struct hhg_cmd_args_subcmd_run {
    char dummy;
} hhg_cmd_args_subcmd_run_t;

typedef struct hhg_cmd_args_subcmd_test {
    bool list;
    bool fail_fast;
    int32_t threads;
    char *pattern;
} hhg_cmd_args_subcmd_test_t;

typedef struct hhg_cmd_args_subcmd_clean {
    char dummy;
} hhg_cmd_args_subcmd_clean_t;

typedef struct hhg_cmd_args_subcmd_repl {
    char dummy;
} hhg_cmd_args_subcmd_repl_t;

typedef union hhg_cmd_args_subcmd {
    hhg_cmd_args_subcmd_init_t init;
    hhg_cmd_args_subcmd_build_t build;
    hhg_cmd_args_subcmd_run_t run;
    hhg_cmd_args_subcmd_test_t test;
    hhg_cmd_args_subcmd_clean_t clean;
    hhg_cmd_args_subcmd_repl_t repl;
} hhg_cmd_args_subcmd_t;

typedef struct hhg_cmd_args {
    hhg_cmd_args_subcmd_type_t subcmd_type;
    hhg_cmd_args_subcmd_t subcmd;
} hhg_cmd_args_t;

hhg_cmd_args_t hhg_cmd_args_parse(int argc, char **argv);

#endif
