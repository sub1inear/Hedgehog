#ifndef HHG_RUN_H
#define HHG_RUN_H

#include <stdbool.h>

typedef struct hhg_type_ctx hhg_type_ctx_t;
typedef struct arena hhg_arena_t;
typedef struct hhg_sym_tab hhg_sym_tab_t;
typedef struct hhg_lexer hhg_lexer_t;
typedef struct hhg_mir_gen hhg_mir_gen_t;

bool hhg_run(const char *filename);

void hhg_run_cleanup(
    hhg_lexer_t *lexer, // required
    hhg_sym_tab_t *sym_tab, // can be NULL
    hhg_arena_t *arena, // can be NULL
    hhg_type_ctx_t *type_ctx, // can be NULL
    hhg_mir_gen_t *mir_gen // can be NULL
);
#endif
