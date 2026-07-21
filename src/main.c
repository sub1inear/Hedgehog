#include <stdbool.h>
#include <stdlib.h>

#include "cmd_args.h"
#include "cmd_exec.h"
#include "main.h"
#include "mem.h"
#include "msg.h"

int hhg_main(int argc, char **argv)
{
    hhg_arena_t *arena = hhg_arena_new();

    hhg_cmd_args_t cmd_args;
    hhg_cmd_args_init(&cmd_args, argc, argv);

    hhg_msg_ctx_t msg_ctx;
    hhg_msg_ctx_init(&msg_ctx, &cmd_args);

    bool result = hhg_cmd_exec_run(&cmd_args, &msg_ctx, arena);

    hhg_msg_ctx_del(&msg_ctx);
    hhg_cmd_args_del(&cmd_args);
    hhg_arena_free(arena);

    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}

int main(int argc, char **argv)
{
    return hhg_main(argc, argv);
}
