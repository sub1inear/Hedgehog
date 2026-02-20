#ifndef HHG_EXT_BUILD_H
#define HHG_EXT_BUILD_H

#include <stdbool.h>

#include "code_gen.h"
#include "cfg.h"

bool hhg_ext_build_run(
    const char **filenames,
    hhg_cfg_backend_t backend
);

#endif
