#ifndef HHG_BUILD_H
#define HHG_BUILD_H

#include <stdbool.h>

typedef struct hhg_cmd_args_build hhg_cmd_args_build_t;
typedef struct hhg_msg_ctx hhg_msg_ctx_t;
typedef struct arena hhg_arena_t;

bool hhg_build(hhg_cmd_args_build_t *build, hhg_msg_ctx_t *msg_ctx,
               hhg_arena_t *arena);

#endif
