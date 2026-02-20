#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "main.h"
#include "mem.h"
#include "cfg.h"
#include "cmd_args.h"
#include "driver.h"

int hhg_main(int argc, char **argv)
{
    hhg_arena_t *arena = hhg_arena_new();

    hhg_cfg_t cfg;
    hhg_cfg_init(&cfg, arena);
    if (hhg_cfg_parse(&cfg, HHG_CONFIG_FILENAME))
        return EXIT_FAILURE;

    hhg_cmd_args_subcmd_t subcmd = hhg_cmd_args_parse(&cfg, argc, argv);

    bool result = hhg_driver_run(&cfg, arena, subcmd);

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
