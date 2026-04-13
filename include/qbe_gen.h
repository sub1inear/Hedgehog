#ifndef HHG_QBE_GEN_H
#define HHG_QBE_GEN_H

#include <stdio.h>

#include "mem.h" // hhg_arena_t is based off arena_t so can't forward declare

typedef struct hhg_code_gen_backend hhg_code_gen_backend_t;
typedef struct hhg_code_gen hhg_code_gen_t;
typedef struct hhg_mir_gen hhg_mir_gen_t;

hhg_code_gen_backend_t *hhg_qbe_gen_backend_new(hhg_arena_t *arena);
void hhg_qbe_gen_backend_run(
    hhg_code_gen_backend_t *backend,
    hhg_code_gen_t *code_gen,
    hhg_mir_gen_t *mir_gen,
    FILE *file
);
void hhg_qbe_gen_backend_print(hhg_code_gen_backend_t *backend);
void hhg_qbe_gen_backend_free(hhg_code_gen_backend_t *backend);

#endif
