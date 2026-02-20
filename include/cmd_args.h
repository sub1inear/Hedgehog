#ifndef HHG_CMD_ARGS_H
#define HHG_CMD_ARGS_H

#include <stdint.h>
#include <stdbool.h>

typedef struct hhg_cfg hhg_cfg_t;

typedef enum hhg_cmd_args_subcmd {
    HHG_CMD_ARGS_SUBCMD_NONE,
    HHG_CMD_ARGS_SUBCMD_INIT,
    HHG_CMD_ARGS_SUBCMD_BUILD,
    HHG_CMD_ARGS_SUBCMD_RUN,
    HHG_CMD_ARGS_SUBCMD_TEST,
    HHG_CMD_ARGS_SUBCMD_CLEAN,
    HHG_CMD_ARGS_SUBCMD_REPL,
} hhg_cmd_args_subcmd_t;

void hhg_cmd_args_parse(hhg_cfg_t *cfg, int argc, char **argv);

#endif
