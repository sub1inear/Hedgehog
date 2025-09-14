#ifndef HHG_MEM_H
#define HHG_MEM_H

#include <stdlib.h>
#include <string.h>

void *hhg_malloc(size_t size);
void *hhg_realloc(void *ptr, size_t size);

void hhg_free(void *ptr);

char *hhg_strdup(const char *str);

#endif