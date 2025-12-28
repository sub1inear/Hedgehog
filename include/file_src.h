#ifndef HHG_FILE_SRC_H
#define HHG_FILE_SRC_H

#include <stdint.h>

typedef struct hhg_file_src {
    char *txt; // terminated with '\0' but EOF is used for tokens
    const char *filename;
    int32_t *line_starts;
} hhg_file_src_t;

void hhg_file_src_init(hhg_file_src_t *src, const char *filename);
void hhg_file_src_del(hhg_file_src_t *src);

#endif
