#ifndef HHG_EXT_BUILD_H
#define HHG_EXT_BUILD_H

#include <stdbool.h>

#include "cfg.h"

typedef struct hhg_msg_ctx hhg_msg_ctx_t;
typedef struct hhg_code_gen hhg_code_gen_t;

typedef struct hhg_ext_build {
    hhg_msg_ctx_t *msg_ctx;
} hhg_ext_build_t;

void hhg_ext_build_init(
    hhg_ext_build_t *ext_build,
    hhg_msg_ctx_t *msg_ctx
);

void hhg_ext_build_run(
    hhg_ext_build_t *ext_build,
    hhg_code_gen_t *code_gen,
    hhg_cfg_backend_t backend
);


#endif
