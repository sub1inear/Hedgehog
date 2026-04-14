#ifndef HHG_CMD_EXEC_H
#define HHG_CMD_EXEC_H

#include "mem.h" // hhg_arena_t is based off arena_t so can't forward declare
#include "cmd_args.h"

typedef struct hhg_cfg hhg_cfg_t;

bool hhg_cmd_exec_run(hhg_cfg_t *cfg, hhg_arena_t *arena);

#endif
