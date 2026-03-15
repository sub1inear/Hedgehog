#ifndef HHG_CPP_GEN_H
#define HHG_CPP_GEN_H

#include <stdio.h>

#include "mem.h" // hhg_arena_t is based off arena_t so can't forward declare

typedef struct hhg_code_gen_backend hhg_code_gen_backend_t;

hhg_code_gen_backend_t *hhg_cpp_gen_backend_new(hhg_arena_t *arena);
void hhg_cpp_gen_backend_print(hhg_code_gen_backend_t *backend);
void hhg_cpp_gen_backend_free(hhg_code_gen_backend_t *backend);

#endif
