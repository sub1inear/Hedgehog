#ifndef HHG_FILE_RANGE_H
#define HHG_FILE_RANGE_H

#include "file_pos.h"

typedef struct hhg_stream hhg_stream_t;

typedef struct hhg_file_range {
    hhg_file_pos_t start;
    hhg_file_pos_t end;
} hhg_file_range_t;

void hhg_file_range_init(hhg_file_range_t *range);

void hhg_file_range_print(hhg_file_range_t *range);
void hhg_file_range_print_stream(
    hhg_file_range_t *range,
    const hhg_stream_t *stream
);

#endif
