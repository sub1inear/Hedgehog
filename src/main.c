#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "main.h"
#include "mem.h"
#include "cfg.h"
#include "cmd_args.h"
#include "cmd_exec.h"

int hhg_main(int argc, char **argv)
{
    hhg_arena_t *arena = hhg_arena_new();
    
    hhg_msg_ctx_t msg_ctx;
    hhg_cfg_t cfg;
    hhg_msg_ctx_init(&msg_ctx, &cfg);
    hhg_cfg_init(&cfg, &msg_ctx, arena);

    if (hhg_cfg_parse(&cfg, HHG_CFG_FILENAME))
        return EXIT_FAILURE;

    hhg_cmd_args_parse(&cfg, argc, argv);

    bool result = hhg_cmd_exec_run(&cfg, &msg_ctx, arena);

    hhg_cfg_del(&cfg);
    hhg_arena_free(arena);
#ifdef HHG_DEBUG_MEM
    hhg_mem_print_summary();
#endif
    return result ? EXIT_FAILURE : EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    return hhg_main(argc, argv);
}
