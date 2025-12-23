#ifndef HHG_FILE_RANGE_H
#define HHG_FILE_RANGE_H

#include "file_pos.h"

typedef struct hhg_file_range {
    hhg_file_pos_t start;
    hhg_file_pos_t end;
} hhg_file_range_t;

void hhg_file_range_init(hhg_file_range_t *range);

void hhg_file_range_print(hhg_file_range_t *range);

#endif