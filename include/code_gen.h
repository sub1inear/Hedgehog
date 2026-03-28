#ifndef HHG_CODE_GEN_H
#define HHG_CODE_GEN_H

#include <stdio.h>

#include "cfg.h"
#include "mem.h" // hhg_arena_t is based off arena_t so can't forward declare

// uses layout trick to simulate inheritance
// first member of all backends must be hhg_code_gen_backend_t
// so that backend can be accessed
// both as a hhg_code_gen_backend_t *
// and as its actual type (with extended data)
typedef struct hhg_code_gen_backend {
    hhg_cfg_backend_t type;
    const char *ext;
    // backend specific data here
} hhg_code_gen_backend_t;

typedef struct hhg_code_gen {
    hhg_code_gen_backend_t *backend;
    char *filename/*[LIBFS_MAX_PATH]*/; // ex. "out/main.cpp"
    char *module; // ex. "main"
    FILE *file;
    hhg_arena_t *arena;
} hhg_code_gen_t;

hhg_code_gen_backend_t *hhg_code_gen_backend_new(
    hhg_arena_t *arena,
    hhg_cfg_backend_t type
);

void hhg_code_gen_init(
    hhg_code_gen_t *gen,
    hhg_code_gen_backend_t *backend,
    const char *in_filename,
    const char *out_dir,
    hhg_arena_t *arena
);

void hhg_code_gen_run(hhg_code_gen_t *code_gen, hhg_mir_gen_t *mir_gen);

void hhg_code_gen_backend_print(hhg_code_gen_backend_t *backend);

void hhg_code_gen_backend_free(hhg_code_gen_backend_t *backend);

void hhg_code_gen_del(hhg_code_gen_t *gen);

#endif
