#include <stdio.h>
#include <inttypes.h>

#include "file_pos.h"

void hhg_file_pos_init(hhg_file_pos_t *pos)
{
    pos->line = 0;
    pos->col = 0;
}

void hhg_file_pos_print(hhg_file_pos_t *pos)
{
    printf("%" PRId32 ":%" PRId32, pos->line + 1, pos->col + 1);
}

void hhg_file_pos_fprint(hhg_file_pos_t *pos, FILE *stream)
{
    fprintf(stream, "%" PRId32 ":%" PRId32, pos->line + 1, pos->col + 1);
}
