#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "build.h"
#include "cmd_args.h"
#include "mem.h"
#include "msg.h"
#include "unit.h"

int main(void)
{
    FILE *file = fopen("fuzz.hhg", "w");
    if (!file)
        hhg_fatal_error("fuzz.hhg: error opening file: %s", strerror(errno));

    srand((unsigned)time(NULL));

    for (int32_t i = 0; i < 1000; i++)
        fputc((char)(rand() % 128), file);

    // put an invalid character to ensure failure
    // (just in case random data is valid)
    fputc('\1', file);

    fclose(file);

    hhg_cmd_args_t cmd_args = {
        .type = HHG_CMD_ARGS_BUILD,
        .subcmd.build =
            {
                .entry = "fuzz.hhg",
                .stop = HHG_CMD_ARGS_STAGE_SEMA,
                .emit = true,
                .warnings = false,
                .error_warnings = false,
            },
    };

    hhg_msg_ctx_t msg_ctx;
    hhg_msg_ctx_init(&msg_ctx, &cmd_args);

    hhg_arena_t *arena = hhg_arena_new();

    // the fuzzed file is expected to cause a failure (or segfault)
    hhg_build(&cmd_args.subcmd.build, &msg_ctx, arena);

    bool failed = (msg_ctx.error_count > 0);

    hhg_arena_free(arena);
    hhg_msg_ctx_del(&msg_ctx);

    return failed ? EXIT_SUCCESS : EXIT_FAILURE;
}
