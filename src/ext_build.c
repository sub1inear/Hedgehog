#include <stdbool.h>

#include "ext_build.h"
#include "cfg.h"
#include "utils.h"

bool hhg_ext_build_run(
    const char **filenames,
    hhg_cfg_build_backend_t backend
)
{
    HHG_UNUSED(filenames, backend);
    return true;
}
