#include <stdbool.h>
#include <stdio.h>

#include <stb_ds.h>

#include "file_src.h"
#include "msg.h"
#include "mem.h"
#include "utils.h"

void hhg_file_src_init(hhg_file_src_t *src, const char *filename)
{
    FILE *file = hhg_fopen(filename, "r");

    // read file into src->txt
    // use realloc instead of fseek + ftell to
    // support stdin and avoid \r\n issues on Windows
    size_t psize = 0;
    size_t fsize = 4096;
    src->txt = NULL; // hhg_realloc(NULL, size) behaves like hhg_malloc(size)
    while (true) {
        // allocate more space
        src->txt = hhg_realloc(src->txt, fsize);

        // read file chunk
        size_t to_read = fsize - psize;
        size_t nread =
            fread(
                src->txt + psize,
                sizeof(src->txt[0]),
                to_read,
                file
            );
        // check if end of file reached
        if (nread < to_read) {
            if (ferror(file))
                hhg_fatal_error(
                    "%s: error reading file: %s",
                    filename,
                    strerror(errno)
                );
            // shrink buffer to actual size + 1 for '\0'
            src->txt = hhg_realloc(src->txt, psize + nread + 1);
            src->txt[psize + nread] = '\0';
            break;
        }
        // double buffer size and save previous size
        // no need to overflow check as size_t is the size of the address space
        psize = fsize;
        fsize *= 2;
    }

    fclose(file);

    src->filename = filename;

    src->line_starts = NULL;
    arrput(src->line_starts, 0);
}

void hhg_file_src_del(hhg_file_src_t *src)
{
    hhg_free(src->txt);
}
