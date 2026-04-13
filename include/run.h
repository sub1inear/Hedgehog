#ifndef HHG_RUN_H
#define HHG_RUN_H

#include <stdbool.h>

#include "mem.h" // hhg_arena_t is based off arena_t so can't forward declare

typedef struct hhg_cfg hhg_cfg_t;

bool hhg_run(hhg_cfg_t *cfg, hhg_arena_t *arena);

#endif
