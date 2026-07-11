#include <stdio.h>
#include <inttypes.h>

#include "file_pos.h"
#include "utils.h"

void hhg_file_pos_init(hhg_file_pos_t *pos)
{
    pos->line = 0;
    pos->col = 0;
}

void hhg_file_pos_print(hhg_file_pos_t *pos)
{
    hhg_file_pos_print_stream(pos, hhg_stream_get_stdout());
}

void hhg_file_pos_print_stream(
    hhg_file_pos_t *pos,
    const hhg_stream_t *stream
)
{
    hhg_stream_printf(
        stream,
        "%i32:%i32",
        pos->line + 1,
        pos->col + 1
    );
}
