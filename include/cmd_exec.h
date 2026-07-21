#ifndef HHG_CMD_EXEC_H
#define HHG_CMD_EXEC_H

#include <stdbool.h>

#include "cmd_args.h"

typedef struct hhg_cmd_args hhg_cmd_args_t;
typedef struct hhg_msg_ctx hhg_msg_ctx_t;
typedef struct arena hhg_arena_t;

bool hhg_cmd_exec_run(hhg_cmd_args_t *cmd_args, hhg_msg_ctx_t *msg_ctx,
                      hhg_arena_t *arena);

#endif
