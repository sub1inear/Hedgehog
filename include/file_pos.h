#ifndef HHG_FILE_POS_H
#define HHG_FILE_POS_H

#include <stdio.h>

#include <stdint.h>

typedef struct hhg_file_pos {
    int32_t line;
    int32_t col;
} hhg_file_pos_t;

void hhg_file_pos_init(hhg_file_pos_t *pos);

void hhg_file_pos_print(hhg_file_pos_t *pos);

void hhg_file_pos_fprint(hhg_file_pos_t *pos, FILE *stream);

#endif
