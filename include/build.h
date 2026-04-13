#ifndef HHG_BUILD_H
#define HHG_BUILD_H

#include <stdbool.h>

#include "mem.h" // hhg_arena_t is based off arena_t so can't forward declare

typedef struct hhg_type_ctx hhg_type_ctx_t;
typedef struct hhg_sym_tab hhg_sym_tab_t;
typedef struct hhg_lexer hhg_lexer_t;
typedef struct hhg_mir_gen hhg_mir_gen_t;
typedef struct hhg_cfg hhg_cfg_t;

bool hhg_build(hhg_cfg_t *cfg, hhg_arena_t *arena);

#endif
