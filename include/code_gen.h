#ifndef HHG_CODE_GEN_H
#define HHG_CODE_GEN_H

#include <stdio.h>

#include "mem.h" // hhg_arena_t is based off arena_t so can't forward declare

typedef struct hhg_sym hhg_sym_t;
typedef struct hhg_mir_instr hhg_mir_instr_t;

typedef struct hhg_code_gen hhg_code_gen_t;

typedef enum hhg_code_gen_backend_type {
    HHG_CODE_GEN_CPP,
    HHG_CODE_GEN_QBE,
} hhg_code_gen_backend_type_t;

typedef struct hhg_code_gen_vtbl {
    void (*start_func)(hhg_code_gen_t *gen, hhg_sym_t *sym);
    void (*emit_instr)(hhg_code_gen_t *gen, hhg_mir_instr_t *instr);
    void (*end_func)(hhg_code_gen_t *gen, hhg_sym_t *sym);
} hhg_code_gen_vtbl_t;

typedef struct hhg_code_gen_backend {
    hhg_code_gen_backend_type_t type;
    hhg_code_gen_vtbl_t vtbl;
    // backend specific data here
} hhg_code_gen_backend_t;

struct hhg_code_gen {
    hhg_code_gen_backend_t *backend;
    FILE *out;
    hhg_arena_t *arena;
};

hhg_code_gen_backend_t *hhg_code_gen_backend_new(
    hhg_arena_t *arena,
    hhg_code_gen_backend_type_t type
);

void hhg_code_gen_init(
    hhg_code_gen_t *gen,
    hhg_code_gen_backend_t *backend,
    const char *filename
);

void hhg_code_gen_run(hhg_code_gen_t *gen);

void hhg_code_gen_backend_free(hhg_code_gen_backend_t *backend);

void hhg_code_gen_del(hhg_code_gen_t *gen);

#endif