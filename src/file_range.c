#include <stdio.h>

#include "file_range.h"
#include "file_pos.h"

void hhg_file_range_init(hhg_file_range_t *range)
{
    hhg_file_pos_init(&range->start);
    hhg_file_pos_init(&range->end);
}

void hhg_file_range_print(hhg_file_range_t *range)
{
    hhg_file_pos_print(&range->start);
    fputs(" - ", stderr);
    hhg_file_pos_print(&range->end);
}
