#include "driver.h"
#include "init.h"
#include "build.h"
#include "repl.h"
#include "msg.h"
#include "utils.h"

bool hhg_driver_run(
    hhg_cfg_t *cfg,
    hhg_arena_t *arena
)
{
    HHG_UNUSED(arena);
    switch (cfg->subcmd) {
        case HHG_CMD_ARGS_SUBCMD_INIT:
            return hhg_init(cfg);
        case HHG_CMD_ARGS_SUBCMD_BUILD:
            // return hhg_build(cfg, arena);
        case HHG_CMD_ARGS_SUBCMD_RUN:
            // return hhg_run(cfg, arena);
        case HHG_CMD_ARGS_SUBCMD_TEST:
            // return hhg_test(cfg, arena);
        case HHG_CMD_ARGS_SUBCMD_CLEAN:
            // return hhg_clean(cfg, arena);
            return true;
        case HHG_CMD_ARGS_SUBCMD_REPL:
            return hhg_repl(cfg);
        default:
            hhg_compiler_error("unknown subcommand: %i", cfg->subcmd);
            return true;
    }
}
