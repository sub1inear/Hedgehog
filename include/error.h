#ifndef HHG_ERROR_H
#define HHG_ERROR_H

typedef enum _hhg_error_type_t {
    ERROR,
    WARNING,
    INFO,
} hhg_msg_type_t;

void hhg_msg(hhg_msg_type_t type, const char *fmt,  ...);

#define hhg_error(...) hhg_msg(ERROR, __VA_ARGS__)
#define hhg_warning(...) hhg_msg(WARNING, __VA_ARGS__);
#define hhg_info(...) hhg_msg(INFO, __VA_ARGS__);

void hhg_fatal_error(const char *fmt, ...);

void hhg_msgs_print();
void hhg_msgs_del();

#endif