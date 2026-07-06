#ifndef HHG_MEM_H
#define HHG_MEM_H

#include <arena.h>

#define hhg_free_s(ptr)                                                        \
    do {                                                                       \
        hhg_free(ptr);                                                         \
        (ptr) = NULL;                                                          \
    } while (0)

typedef arena_t hhg_arena_t;

void *hhg_malloc(size_t size);

void *hhg_realloc(void *ptr, size_t size);

void hhg_free(void *ptr);

char *hhg_strdup(const char *str);

hhg_arena_t *hhg_arena_new(void);

void *hhg_arena_malloc(hhg_arena_t *arena, size_t size);

char *hhg_arena_strdup(hhg_arena_t *arena, const char *str);

void hhg_arena_free(hhg_arena_t *arena);

// debugging versions
// notes:
//  - assumes printf does not use any hhg allocation functions
//  - not thread safe
#ifdef HHG_MEM_DEBUG
typedef struct hhg_mem_alloc_loc {
    const char *file;
    const char *func;
    int line;
} hhg_mem_alloc_loc_t;

// do not use:
void *hhg_debug_malloc(size_t size, hhg_mem_alloc_loc_t loc);
void *hhg_debug_realloc(void *ptr, size_t size, hhg_mem_alloc_loc_t loc);
void hhg_debug_free(void *ptr, hhg_mem_alloc_loc_t loc);

char *hhg_debug_strdup(const char *str, hhg_mem_alloc_loc_t loc);

hhg_arena_t *hhg_debug_arena_new(hhg_mem_alloc_loc_t loc);
void *hhg_debug_arena_malloc(hhg_arena_t *arena, size_t size,
                             hhg_mem_alloc_loc_t loc);
char *hhg_debug_arena_strdup(hhg_arena_t *arena, const char *str,
                             hhg_mem_alloc_loc_t loc);
void hhg_debug_arena_free(hhg_arena_t *arena, hhg_mem_alloc_loc_t loc);

// can use:
void hhg_mem_debug_print_stats(void);

#define hhg_malloc(size)                                                       \
    hhg_debug_malloc(size,                                                     \
                     (hhg_mem_alloc_loc_t){ __FILE__, __func__, __LINE__ })
#define hhg_realloc(ptr, size)                                                 \
    hhg_debug_realloc(ptr, size,                                               \
                      (hhg_mem_alloc_loc_t){ __FILE__, __func__, __LINE__ })
#define hhg_free(ptr)                                                          \
    hhg_debug_free(ptr, (hhg_mem_alloc_loc_t){ __FILE__, __func__, __LINE__ })
#define hhg_strdup(str)                                                        \
    hhg_debug_strdup(str, (hhg_mem_alloc_loc_t){ __FILE__, __func__, __LINE__ })
#define hhg_arena_new()                                                        \
    hhg_debug_arena_new((hhg_mem_alloc_loc_t){ __FILE__, __func__, __LINE__ })
#define hhg_arena_malloc(arena, size)                                          \
    hhg_debug_arena_malloc(                                                    \
        arena, size, (hhg_mem_alloc_loc_t){ __FILE__, __func__, __LINE__ })
#define hhg_arena_strdup(arena, str)                                           \
    hhg_debug_arena_strdup(                                                    \
        arena, str, (hhg_mem_alloc_loc_t){ __FILE__, __func__, __LINE__ })
#define hhg_arena_free(arena)                                                  \
    hhg_debug_arena_free(                                                      \
        arena, (hhg_mem_alloc_loc_t){ __FILE__, __func__, __LINE__ })
#endif

#endif
