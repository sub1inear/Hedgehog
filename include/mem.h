#ifndef HHG_MEM_H
#define HHG_MEM_H

#include <arena.h>

#define hhg_free_s(ptr) do { hhg_free(ptr); (ptr) = NULL; } while (0)

typedef arena_t hhg_arena_t;

void *hhg_malloc(size_t size);

void *hhg_realloc(void *ptr, size_t size);

void hhg_free(void *ptr);

char *hhg_strdup(const char *str);

hhg_arena_t *hhg_arena_new(void);

void *hhg_arena_malloc(hhg_arena_t *arena, size_t size);

char *hhg_arena_strdup(hhg_arena_t *arena, const char *str);

void hhg_arena_free(hhg_arena_t *arena);

#endif
