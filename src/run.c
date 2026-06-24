#include <stdbool.h>
#include <stdlib.h>

#include <fs.h>

#include "run.h"
#include "cfg.h"
#include "str.h"
#include "utils.h"

bool hhg_run(hhg_cfg_t *cfg)
{
    if (HHG_EXEC_EXT[0] == '\0')
        return hhg_utils_spawn(
            (const char *[]) { cfg->project.name, NULL },
            NULL
        ) != EXIT_SUCCESS;
    else {
        hhg_str_t str;
        hhg_str_init_str(&str, cfg->project.name);
        hhg_str_append_str(&str, HHG_EXEC_EXT);
        int exit_code =
            hhg_utils_spawn((const char *[]){ str.str, NULL }, NULL);
        hhg_str_del(&str);
        return exit_code != EXIT_SUCCESS;
    }
}
