#include <stdlib.h>
#ifdef HHG_DEBUG_MEM
#include <stdio.h>
#endif

#include "mem.h"
#include "error.h"

void *hhg_malloc(size_t size)
{
    void *ptr = malloc(size);
    if (ptr == NULL) {
        hhg_fatal_error("memory allocation failed");
    }
#ifdef HHG_DEBUG_MEM
    printf("alloc: %p (%zu bytes)\n", ptr, size);
#endif
    return ptr;
}

void *hhg_realloc(void *ptr, size_t size)
{
    void *next = realloc(ptr, size);
    if (next == NULL) {
        hhg_free(ptr);
        hhg_fatal_error("memory allocation failed");
    }
#ifdef HHG_DEBUG_MEM
    printf("realloc: %p -> %p (%zu bytes)\n", ptr, next, size);
#endif
    return next;
}

void hhg_free(void *ptr)
{
#ifdef HHG_DEBUG_MEM
    printf("free: %p\n", ptr);
#endif
    free(ptr);
}

char *hhg_strdup(const char *str)
{
    size_t len = strlen(str) + 1;
    char *buf = hhg_malloc(len);
    return strcpy(buf, str);
}
