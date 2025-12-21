#include <stdlib.h>
#ifdef HHG_DEBUG_MEM
#include <stdio.h>
#endif

#define ARENA_API static
#define ARENA_IMPLEMENTATION
#include <arena.h>

#include "mem.h"
#include "msg.h"

void *hhg_malloc(size_t size)
{
    void *ptr = malloc(size);
    if (ptr == NULL)
        hhg_fatal_error("memory allocation failed");

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
    free((void *)ptr);
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
#ifdef HHG_DEBUG_MEM
    printf("arena created: %p\n", arena);
#endif
    return arena;
}

void *hhg_arena_malloc(hhg_arena_t *arena, size_t size)
{
    void *ptr = arena_malloc(arena, size);
    if (ptr == NULL)
        hhg_fatal_error("arena allocation failed");
#ifdef HHG_DEBUG_MEM
    printf("arena alloc: %p (%zu bytes)\n", ptr, size);
#endif
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
#ifdef HHG_DEBUG_MEM
    printf("arena destroyed: %p\n", arena);
#endif
    arena_destroy(arena);
}