#include <stdbool.h>
#include <stdlib.h>

#include <fs.h>

#include "run.h"
#include "cfg.h"
#include "utils.h"

bool hhg_run(hhg_cfg_t *cfg)
{
    return hhg_utils_system("%s" HHG_UTILS_EXEC_EXT, cfg->project.name) == EXIT_FAILURE;
}
