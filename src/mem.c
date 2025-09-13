#include <stdlib.h>

#include "mem.h"
#include "error.h"

void *hhg_malloc(size_t size)
{
    void *ptr = malloc(size);
    if (ptr == NULL) {
        hhg_fatal_error("memory allocation failed");
    }
    return ptr;
}

void *hhg_realloc(void *ptr, size_t size)
{
    void *next = realloc(ptr, size);
    if (next == NULL) {
        hhg_free(ptr);
        hhg_fatal_error("memory allocation failed");
    }
    return next;
}

void hhg_free(void *ptr)
{
    free(ptr);
}