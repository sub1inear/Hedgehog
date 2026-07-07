#ifndef HHG_EXT_BUILD_H
#define HHG_EXT_BUILD_H

#include <stdbool.h>

typedef struct hhg_msg_ctx hhg_msg_ctx_t;
typedef struct hhg_code_gen hhg_code_gen_t;
typedef struct hhg_ext_build_cxx_data hhg_ext_build_cxx_data_t;
typedef struct arena hhg_arena_t;

typedef struct hhg_ext_build {
    const char *cxx;
    const hhg_ext_build_cxx_data_t *cxx_data;
    hhg_msg_ctx_t *msg_ctx;
    hhg_arena_t *arena;
} hhg_ext_build_t;

void hhg_ext_build_init(
    hhg_ext_build_t *ext_build,
    hhg_msg_ctx_t *msg_ctx,
    hhg_arena_t *arena
);

void hhg_ext_build_run(
    hhg_ext_build_t *ext_build,
    hhg_code_gen_t *code_gen,
    const char *filename,
    const char *orig_filename,
    const char *out,
    bool release
);


#endif
