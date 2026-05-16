#ifndef HHG_BUILD_H
#define HHG_BUILD_H

#include <stdbool.h>

#include "mem.h" // hhg_arena_t is based off arena_t so can't forward declare

typedef struct hhg_cfg hhg_cfg_t;
typedef struct hhg_msg_ctx hhg_msg_ctx_t;

bool hhg_build(hhg_cfg_t *cfg, hhg_msg_ctx_t *msg_ctx, hhg_arena_t *arena);

#endif
