#ifndef HHG_MSG_H
#define HHG_MSG_H

#include <stdint.h>

typedef struct hhg_file_range hhg_file_range_t;
typedef struct hhg_file_src hhg_file_src_t;
typedef struct hhg_cfg hhg_cfg_t;

typedef struct hhg_msg_ctx {
    int32_t error_count;
    hhg_cfg_t *cfg;
} hhg_msg_ctx_t;

typedef enum hhg_msg_type {
    HHG_MSG_ERROR,
    HHG_MSG_WARNING,
    HHG_MSG_INFO,
} hhg_msg_type_t;

// note: cfg may be uninitialized
void hhg_msg_ctx_init(hhg_msg_ctx_t *msg_ctx, hhg_cfg_t *cfg);

// note: in the future, hhg_msg and hhg_basic_msg may be buffered

void hhg_msg(
    hhg_msg_ctx_t *msg_ctx,
    hhg_msg_type_t type,
    hhg_file_src_t *src,
    hhg_file_range_t *range,
    const char *msg, // main message
    const char *note, // additional note (can be NULL)
    ... // format arguments for both msg and note
);

void hhg_basic_msg(
    hhg_msg_ctx_t *msg_ctx,
    hhg_msg_type_t type,
    const char *msg,
    ...
);

void hhg_compiler_error(const char *msg, ...);
void hhg_fatal_error(const char *msg, ...);

#endif
