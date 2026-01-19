#include <stdbool.h>

#include "ext_build.h"
#include "utils.h"

bool hhg_ext_build_run(
    const char **filenames,
    hhg_code_gen_backend_type_t backend
)
{
    HHG_UNUSED(filenames, backend);
    return true;
}
