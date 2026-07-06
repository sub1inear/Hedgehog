#include "cmd_exec.h"

#include <stdbool.h>

#include "build.h"
#include "cmd_args.h"
#include "init.h"
#include "msg.h"
#include "run.h"

bool hhg_cmd_exec_run(hhg_cmd_args_t *cmd_args, hhg_msg_ctx_t *msg_ctx,
                      hhg_arena_t *arena)
{
    switch (cmd_args->type) {
    case HHG_CMD_ARGS_INIT:
        return hhg_init(&cmd_args->subcmd.init);
    case HHG_CMD_ARGS_BUILD:
        return hhg_build(&cmd_args->subcmd.build, msg_ctx, arena);
    case HHG_CMD_ARGS_RUN:
        return hhg_run(&cmd_args->subcmd.run, msg_ctx, arena);
    default:
        hhg_compiler_error("unknown subcommand: %i", cmd_args->type);
        return true;
    }
}
