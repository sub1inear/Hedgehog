#ifndef HHG_EXT_BUILD_H
#define HHG_EXT_BUILD_H

#include <stdbool.h>

#include "code_gen.h"

bool hhg_ext_build_run(
    const char **filenames,
    hhg_code_gen_backend_type_t backend
);

#endif
