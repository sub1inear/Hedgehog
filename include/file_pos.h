#ifndef HHG_FILE_POS_H
#define HHG_FILE_POS_H

#include <stdint.h>

typedef struct _hhg_file_pos_t {
    int32_t line;
    int32_t col;
} hhg_file_pos_t;

void hhg_file_pos_init(hhg_file_pos_t *pos);

#endif
