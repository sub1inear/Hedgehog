#include <stdio.h>

#include "file_range.h"
#include "file_pos.h"
#include "str.h"
#include "utils.h"

void hhg_file_range_init(hhg_file_range_t *range)
{
    hhg_file_pos_init(&range->start);
    hhg_file_pos_init(&range->end);
}

void hhg_file_range_print(hhg_file_range_t *range)
{
    hhg_file_range_fprint(range, stdout);
}

void hhg_file_range_fprint(hhg_file_range_t *range, FILE *stream)
{
    hhg_file_pos_fprint(&range->start, stream);
    fputs(" - ", stream);
    hhg_file_pos_fprint(&range->end, stream);
}
