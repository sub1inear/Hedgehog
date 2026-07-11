#ifndef HHG_FILE_POS_H
#define HHG_FILE_POS_H

#include <stdint.h>

typedef struct hhg_stream hhg_stream_t;

typedef struct hhg_file_pos {
    int32_t line;
    int32_t col;
} hhg_file_pos_t;

void hhg_file_pos_init(hhg_file_pos_t *pos);

void hhg_file_pos_print(hhg_file_pos_t *pos);
void hhg_file_pos_print_stream(
    hhg_file_pos_t *pos,
    const hhg_stream_t *stream
);

#endif
