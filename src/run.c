#include "run.h"

#include <stb_ds.h>
#include <stdbool.h>
#include <stdlib.h>

#include "build.h"
#include "cmd_args.h"
#include "utils.h"

bool hhg_run(hhg_cmd_args_run_t *run, hhg_msg_ctx_t *msg_ctx,
             hhg_arena_t *arena)
{
    if (hhg_build(&run->build, msg_ctx, arena))
        return false;

    const char **argv = NULL;

    const char *exec_path = hhg_utils_file_to_exec(arena, run->build.entry);
    arrput(argv, exec_path);

    arrcat(argv, run->args);
    arrput(argv, NULL);

    int exit_code = hhg_utils_spawn(argv, NULL);
    arrfree(argv);

    return exit_code == EXIT_SUCCESS;
}
