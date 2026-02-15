#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "cfg.h"
#include "cmd_args.h"
#include "mem.h"

int main(int argc, char **argv)
{
    hhg_arena_t *arena = hhg_arena_new();

    hhg_cfg_t cfg;
    hhg_cfg_init(&cfg, arena);
    hhg_cfg_parse(&cfg, "hhg.toml");
    hhg_cmd_args_parse(&cfg, argc, argv);

    // hhg_driver_run(&cfg, arena);

    hhg_cfg_del(&cfg);
    hhg_arena_free(arena);
#ifdef HHG_DEBUG_MEM
    hhg_mem_print_summary();
#endif
    return EXIT_SUCCESS;
}
