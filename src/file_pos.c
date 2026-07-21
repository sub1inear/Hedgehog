#include <inttypes.h>
#include <stdio.h>

#include "file_pos.h"
#include "str.h"
#include "utils.h"

void hhg_file_pos_init(hhg_file_pos_t *pos)
{
    pos->line = 0;
    pos->col = 0;
}

void hhg_file_pos_print(hhg_file_pos_t *pos)
{
    hhg_file_pos_fprint(pos, stdout);
}

void hhg_file_pos_fprint(hhg_file_pos_t *pos, FILE *stream)
{
    fprintf(stream, "%" PRId32 ":%" PRId32, pos->line + 1, pos->col + 1);
}
