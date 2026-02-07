#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "build.h"
#include "cmd_args.h"
#include "mem.h"
#include "utils.h"

int main(int argc, char **argv)
{
    hhg_cmd_args_t cmd_args = hhg_cmd_args_parse(argc, argv);
    HHG_UNUSED(cmd_args);

    hhg_mem_print_summary();
}
