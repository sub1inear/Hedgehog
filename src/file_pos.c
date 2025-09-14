#include "file_pos.h"

void hhg_file_pos_init(hhg_file_pos_t *pos)
{
    pos->line = 1;
    pos->col = 1;
}