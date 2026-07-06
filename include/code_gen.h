#ifndef HHG_CODE_GEN_H
#define HHG_CODE_GEN_H

typedef struct hhg_mir_gen hhg_mir_gen_t;
typedef struct arena hhg_arena_t;

typedef struct hhg_code_gen {
    hhg_arena_t *arena;
} hhg_code_gen_t;

void hhg_code_gen_init(
    hhg_code_gen_t *gen,
    hhg_arena_t *arena
);

bool hhg_code_gen_run(
    hhg_code_gen_t *code_gen,
    hhg_mir_gen_t *mir_gen,
    const char *filename,
    const char **out_filename
);

void hhg_code_gen_print(hhg_code_gen_t *gen);

void hhg_code_gen_del(hhg_code_gen_t *gen);

#endif
