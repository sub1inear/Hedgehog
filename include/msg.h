#ifndef HHG_MSG_H
#define HHG_MSG_H

#include <stdint.h>

#include "file_pos.h"

typedef enum hhg_msg_type {
    HHG_MSG_ERROR,
    HHG_MSG_WARNING,
    HHG_MSG_INFO,
} hhg_msg_type_t;

void hhg_msg(hhg_msg_type_t type,
             hhg_file_pos_t pos,
             const char *filename,
             const char *fmt,
             ...);

#define hhg_error(...) hhg_msg(HHG_MSG_ERROR, __VA_ARGS__)
#define hhg_warning(...) hhg_msg(HHG_MSG_WARNING, __VA_ARGS__)
#define hhg_info(...) hhg_msg(HHG_MSG_INFO, __VA_ARGS__)

void hhg_fatal_error(const char *fmt, ...);

int32_t hhg_msgs_get_error_count(void);

#endif
