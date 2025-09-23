#ifndef HHG_ERROR_H
#define HHG_ERROR_H

#include <stdbool.h>

#include "file_pos.h"

enum _hhg_msg_type_t {
    HHG_MSG_ERROR,
    HHG_MSG_WARNING,
    HHG_MSG_INFO,
};
typedef int hhg_msg_type_t;

void hhg_msg(hhg_msg_type_t type,
             hhg_file_pos_t pos,
             const char *filename,
             const char *fmt,
             ...);

#define hhg_error(...) hhg_msg(HHG_MSG_ERROR, __VA_ARGS__)
#define hhg_warning(...) hhg_msg(HHG_MSG_WARNING, __VA_ARGS__)
#define hhg_info(...) hhg_msg(HHG_MSG_INFO, __VA_ARGS__)

void hhg_fatal_error(const char *fmt, ...);

bool hhg_msgs_has_errors(void);
void hhg_msgs_print(void);
void hhg_msgs_del(void);

#endif
