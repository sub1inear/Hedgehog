#ifndef HHG_RUN_H
#define HHG_RUN_H

#include <stdbool.h>

typedef struct hhg_cmd_args_run hhg_cmd_args_run_t;
typedef struct hhg_msg_ctx hhg_msg_ctx_t;
typedef struct arena hhg_arena_t;

bool hhg_run(
    hhg_cmd_args_run_t *run,
    hhg_msg_ctx_t *msg_ctx,
    hhg_arena_t *arena
);

#endif
