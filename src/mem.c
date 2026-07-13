#define ARENA_API static
#define ARENA_IMPLEMENTATION
#include <arena.h>

#include "mem.h"

#define STB_DS_IMPLEMENTATION
#define STBDS_REALLOC(context, ptr, size) hhg_realloc(ptr, size)
#define STBDS_FREE(context, ptr) hhg_free(ptr)
#include <stb_ds.h>

#include "msg.h"

#undef hhg_malloc
#undef hhg_realloc
#undef hhg_free
#undef hhg_strdup
#undef hhg_arena_new
#undef hhg_arena_malloc
#undef hhg_arena_strdup
#undef hhg_arena_free

void *hhg_malloc(size_t size)
{
    void *ptr = malloc(size);
    if (ptr == NULL)
        hhg_fatal_error("memory allocation failed");
    return ptr;
}

void *hhg_realloc(void *ptr, size_t size)
{
    void *next = realloc(ptr, size);
    if (next == NULL) {
        hhg_free_s(ptr);
        hhg_fatal_error("memory allocation failed");
    }
    return next;
}

void hhg_free(void *ptr)
{
    free(ptr);
}

char *hhg_strdup(const char *str)
{
    size_t len = strlen(str) + 1;
    char *buf = hhg_malloc(len);
    return strcpy(buf, str);
}

hhg_arena_t *hhg_arena_new(void)
{
    arena_t *arena = arena_create();
    if (arena == NULL)
        hhg_fatal_error("arena creation failed");
    return arena;
}

void *hhg_arena_malloc(hhg_arena_t *arena, size_t size)
{
    void *ptr = arena_malloc(arena, size);
    if (ptr == NULL)
        hhg_fatal_error("arena allocation failed");
    return ptr;
}

char *hhg_arena_strdup(hhg_arena_t *arena, const char *str)
{
    size_t len = strlen(str) + 1;
    char *buf = hhg_arena_malloc(arena, len);
    return strcpy(buf, str);
}

void hhg_arena_free(hhg_arena_t *arena)
{
    arena_destroy(arena);
}
