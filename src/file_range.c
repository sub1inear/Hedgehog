#include <stdio.h>

#include "file_range.h"
#include "file_pos.h"
#include "utils.h"

void hhg_file_range_init(hhg_file_range_t *range)
{
    hhg_file_pos_init(&range->start);
    hhg_file_pos_init(&range->end);
}

void hhg_file_range_print(hhg_file_range_t *range)
{
    hhg_file_range_print_stream(range, hhg_stream_get_stdout());
}

void hhg_file_range_print_stream(
    hhg_file_range_t *range,
    const hhg_stream_t *stream
)
{
    hhg_stream_printf(stream, "%P - %P", &range->start, &range->end);
}
