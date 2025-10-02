#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "debug.h"
#include "run.h"
#include "cmd_args.h"

int main(int argc, char **argv)
{
    (void)argc;
    hhg_cmd_args_parse(argv);

    hhg_cmd_args_t *cmd_args = hhg_cmd_args_get();

    bool result = false;

    if (cmd_args->filename != NULL)
        result = hhg_run(cmd_args->filename);

    return result ? EXIT_FAILURE : EXIT_SUCCESS;
}
