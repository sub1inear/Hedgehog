#include "cmd_exec.h"
#include "init.h"
#include "build.h"
#include "run.h"
#include "clean.h"
#include "repl.h"
#include "msg.h"
#include "utils.h"

bool hhg_cmd_exec_run(
    hhg_cfg_t *cfg,
    hhg_msg_ctx_t *msg_ctx,
    hhg_arena_t *arena
)
{
    HHG_UNUSED(arena);
    switch (cfg->subcmd) {
        case HHG_CMD_ARGS_SUBCMD_INIT:
            return hhg_init(cfg);
        case HHG_CMD_ARGS_SUBCMD_BUILD:
            return hhg_build(cfg, msg_ctx, arena);
        case HHG_CMD_ARGS_SUBCMD_RUN:
            return hhg_run(cfg);
        case HHG_CMD_ARGS_SUBCMD_TEST:
            // return hhg_test(cfg, arena);
            return true;
        case HHG_CMD_ARGS_SUBCMD_CLEAN:
            return hhg_clean(cfg, msg_ctx);
        case HHG_CMD_ARGS_SUBCMD_REPL:
            hhg_repl();
            return true;
        default:
            hhg_compiler_error("unknown subcommand: %i", cfg->subcmd);
            return true;
    }
}
