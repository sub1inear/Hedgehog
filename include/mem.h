#ifndef HHG_MEM_H
#define HHG_MEM_H

#include <stdlib.h>
#include <string.h>

#define hhg_free_s(ptr) do { hhg_free(ptr); (ptr) = NULL; } while (0)

void *hhg_malloc(size_t size);
void *hhg_realloc(void *ptr, size_t size);

void hhg_free(void *ptr);

char *hhg_strdup(const char *str);

#endif
