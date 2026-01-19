#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "build.h"
#include "cmd_args.h"
#include "mem.h"
#include "utils.h"

int main(int argc, char **argv)
{
    HHG_UNUSED(argc);
    hhg_cmd_args_parse(argv);

    hhg_cmd_args_t *cmd_args = hhg_cmd_args_get();

    bool result = false;

    if (cmd_args->filename != NULL)
        result = hhg_build_debug(cmd_args->filename, HHG_BUILD_STAGE_MIR);

    hhg_mem_print_summary();

    return result ? EXIT_FAILURE : EXIT_SUCCESS;
}
