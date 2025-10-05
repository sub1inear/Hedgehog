#ifndef HHG_CMD_ARGS_H
#define HHG_CMD_ARGS_H

#include <stdint.h>
#include <stdbool.h>

typedef struct _hhg_cmd_args_t {
    bool warnings;
    bool heap;
    char *run_cmd;
    char **pass;
    char *filename;
} hhg_cmd_args_t;

void hhg_cmd_args_parse(char **argv);
hhg_cmd_args_t *hhg_cmd_args_get(void);

#endif