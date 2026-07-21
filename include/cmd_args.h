#ifndef HHG_CMD_ARGS_H
#define HHG_CMD_ARGS_H

#include <stdbool.h>

typedef enum hhg_cmd_args_type {
    HHG_CMD_ARGS_NONE,
    HHG_CMD_ARGS_INIT,
    HHG_CMD_ARGS_BUILD,
    HHG_CMD_ARGS_RUN,
} hhg_cmd_args_type_t;

typedef struct hhg_cmd_args_init {
    const char *name;
} hhg_cmd_args_init_t;

typedef enum hhg_cmd_args_stage {
    HHG_CMD_ARGS_STAGE_NONE,
    HHG_CMD_ARGS_STAGE_LEXER,
    HHG_CMD_ARGS_STAGE_PARSER,
    HHG_CMD_ARGS_STAGE_SEM_AN,
    HHG_CMD_ARGS_STAGE_MIR_GEN,
    HHG_CMD_ARGS_STAGE_MEM_AN,
    HHG_CMD_ARGS_STAGE_CODE_GEN,
    HHG_CMD_ARGS_STAGE_EXT_BUILD,
} hhg_cmd_args_stage_t;

typedef struct hhg_cmd_args_build {
    const char *entry;
    const char *out;
    bool release;
    hhg_cmd_args_stage_t stop;
    bool emit;
    bool warnings;
    bool error_warnings;
} hhg_cmd_args_build_t;

typedef struct hhg_cmd_args_run {
    hhg_cmd_args_build_t build;
    const char **args;
} hhg_cmd_args_run_t;

typedef union hhg_cmd_args_subcmd {
    hhg_cmd_args_init_t init;
    hhg_cmd_args_build_t build;
    hhg_cmd_args_run_t run;
} hhg_cmd_args_subcmd_t;

typedef struct hhg_cmd_args {
    hhg_cmd_args_type_t type;
    hhg_cmd_args_subcmd_t subcmd;
} hhg_cmd_args_t;

void hhg_cmd_args_init(hhg_cmd_args_t *cmd_args, int argc, char **argv);
void hhg_cmd_args_del(hhg_cmd_args_t *cmd_args);

#endif
