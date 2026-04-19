#include <stdbool.h>
#include <stdlib.h>

#include <fs.h>

#include "run.h"
#include "cfg.h"
#include "utils.h"

bool hhg_run(hhg_cfg_t *cfg, hhg_arena_t *arena)
{
    HHG_UNUSED(arena);
    // use %s to avoid security risk for printing user strings
    return hhg_utils_system("%s", cfg->project.name) == EXIT_FAILURE;
}
