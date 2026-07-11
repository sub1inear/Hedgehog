#include <stdbool.h>

#define ARENA_API static
#define ARENA_IMPLEMENTATION
#include <arena.h>

#include "mem.h"

#define STB_DS_IMPLEMENTATION
#define STBDS_REALLOC(context, ptr, size) hhg_realloc(ptr, size)
#define STBDS_FREE(context, ptr) hhg_free(ptr)
#include <stb_ds.h>

#include "msg.h"
#include "utils.h"

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

#ifdef HHG_MEM_DEBUG

// note: no hhg_printf in this file to avoid allocations
// hhg_printf could use hhg_malloc

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <fs.h>

#include "utils.h"

typedef struct hhg_mem_alloc {
    void *key;
    hhg_mem_alloc_loc_t *value; // dynamic array of (re)allocations                  
} hhg_mem_alloc_t;

typedef struct hhg_mem_arena_alloc {
    void *key;
    hhg_mem_alloc_loc_t value; // location of arena creation
} hhg_mem_arena_alloc_t;


static void hhg_mem_alloc_tab_add(void *ptr, hhg_mem_alloc_loc_t loc);

static void hhg_mem_alloc_loc_print(hhg_mem_alloc_loc_t loc);

// runs func with mem_debug enabled
// func should print allocation info and
// update mem_alloc_tab/mem_arena_alloc_tab as needed
// returns ptr1 for convenience of wrapping allocation functions
static void *hhg_mem_debug_run_func(
    void (*func)(
        void *ptr1,
        void *ptr2,
        size_t size,
        hhg_mem_alloc_loc_t loc
    ),
    void *ptr1,
    void *ptr2,
    size_t size,
    hhg_mem_alloc_loc_t loc,
    const char *func_name,
    size_t *func_counter,
    bool is_realloc
);
static void hhg_debug_malloc_core(
    void *ptr,
    void *null,
    size_t size,
    hhg_mem_alloc_loc_t loc
);
static void hhg_debug_realloc_core(
    void *next,
    void *ptr,
    size_t size,
    hhg_mem_alloc_loc_t loc
);
static void hhg_debug_free_core(
    void *ptr,
    void *null,
    size_t size,
    hhg_mem_alloc_loc_t loc
);
static void hhg_debug_strdup_core(
    void *buf,
    void *null,
    size_t size,
    hhg_mem_alloc_loc_t loc
);
static void hhg_debug_arena_new_core(
    void *arena,
    void *null,
    size_t size,
    hhg_mem_alloc_loc_t loc
);
static void hhg_debug_arena_malloc_core(
    void *ptr,
    void *arena,
    size_t size,
    hhg_mem_alloc_loc_t loc
);
static void hhg_debug_arena_strdup_core(
    void *buf,
    void *arena,
    size_t size,
    hhg_mem_alloc_loc_t loc
);
static void hhg_debug_arena_free_core(
    void *arena,
    void *null,
    size_t size,
    hhg_mem_alloc_loc_t loc
);

// hashmap of allocations to their allocation locations
static hhg_mem_alloc_t *mem_alloc_tab = NULL;

// hashmap of arena allocations to their allocation location
static hhg_mem_arena_alloc_t *mem_arena_alloc_tab = NULL;

// to prevent recursion in debug functions
// not perfect but is the simplest way
static bool mem_debug = false;

// allocation statistics
static size_t mem_mallocs = 0;
static size_t mem_reallocs = 0;
static size_t mem_frees = 0;
static size_t mem_strdups = 0;
static size_t mem_arena_news = 0;
static size_t mem_arena_mallocs = 0;
static size_t mem_arena_strdups = 0;
static size_t mem_arena_frees = 0;

void *hhg_debug_malloc(size_t size, hhg_mem_alloc_loc_t loc)
{
    void *ptr = hhg_malloc(size);

    return hhg_mem_debug_run_func(
        hhg_debug_malloc_core,
        ptr,
        NULL,
        size,
        loc,
        "malloc",
        &mem_mallocs,
        false
    );
}

void *hhg_debug_realloc(void *ptr, size_t size, hhg_mem_alloc_loc_t loc)
{
    void *next = hhg_realloc(ptr, size);
    
    return hhg_mem_debug_run_func(
        hhg_debug_realloc_core,
        next,
        ptr,
        size,
        loc,
        "realloc",
        &mem_reallocs,
        true
    );
}

void hhg_debug_free(void *ptr, hhg_mem_alloc_loc_t loc)
{
    hhg_mem_debug_run_func(
        hhg_debug_free_core,
        ptr,
        NULL,
        0,
        loc,
        "free",
        &mem_frees,
        false
    );
    // free after in case free zeros out ptr
    hhg_free(ptr);
}

char *hhg_debug_strdup(const char *str, hhg_mem_alloc_loc_t loc)
{
    char *buf = hhg_strdup(str);

    return hhg_mem_debug_run_func(
        hhg_debug_strdup_core,
        buf,
        NULL,
        strlen(str) + 1,
        loc,
        "strdup",
        &mem_strdups,
        false
    );
}

hhg_arena_t *hhg_debug_arena_new(hhg_mem_alloc_loc_t loc)
{
    arena_t *arena = hhg_arena_new();
    
    return hhg_mem_debug_run_func(
        hhg_debug_arena_new_core,
        arena,
        NULL,
        0,
        loc,
        "arena new",
        &mem_arena_news,
        false
    );
}

void *hhg_debug_arena_malloc(hhg_arena_t *arena, size_t size, hhg_mem_alloc_loc_t loc)
{
    void *ptr = hhg_arena_malloc(arena, size);
    
    return hhg_mem_debug_run_func(
        hhg_debug_arena_malloc_core,
        ptr,
        arena,
        size,
        loc,
        "arena malloc",
        &mem_arena_mallocs,
        false
    );
}

char *hhg_debug_arena_strdup(hhg_arena_t *arena, const char *str, hhg_mem_alloc_loc_t loc)
{
    char *buf = hhg_arena_strdup(arena, str);
    
    return hhg_mem_debug_run_func(
        hhg_debug_arena_strdup_core,
        buf,
        arena,
        strlen(str) + 1,
        loc,
        "arena strdup",
        &mem_arena_strdups,
        false
    );
}

void hhg_debug_arena_free(hhg_arena_t *arena, hhg_mem_alloc_loc_t loc)
{
    hhg_mem_debug_run_func(
        hhg_debug_arena_free_core,
        arena,
        NULL,
        0,
        loc,
        "arena free",
        &mem_arena_frees,
        false
    );

    // destroy after in case destroy zeros out arena
    arena_destroy(arena);
}

void hhg_mem_debug_print_stats(void)
{
    bool prev_mem_debug = mem_debug;
    mem_debug = true;
    
    size_t num_mem_leaks = hmlenu(mem_alloc_tab);
    if (num_mem_leaks > 0) {
        printf("%zu memory leaks detected:\n", num_mem_leaks);
        for (size_t i = 0; i < num_mem_leaks; i++) {
            hhg_mem_alloc_t alloc = mem_alloc_tab[i];
            hhg_mem_alloc_loc_t *locs = alloc.value;

            size_t locs_len = arrlenu(locs);
            hhg_assert(locs_len > 0);
            if (locs_len == 1) {
                printf("  leak at %p allocated at ", alloc.key);
                hhg_mem_alloc_loc_print(locs[0]);
                putchar('\n');
            } else {
                printf("  leak at %p allocated/reallocated at:\n", alloc.key);
                for (size_t j = 0; j < locs_len; j++) {
                    fputs("    ", stdout);
                    hhg_mem_alloc_loc_print(locs[j]);
                    putchar('\n');
                }
            }

            arrfree(locs);
        }
    }

    hmfree(mem_alloc_tab);

    size_t num_arena_leaks = hmlen(mem_arena_alloc_tab);
    if (num_arena_leaks > 0) {
        printf("%zu arena leaks detected:\n", num_arena_leaks);
        for (size_t i = 0; i < num_arena_leaks; i++) {
            hhg_mem_arena_alloc_t alloc = mem_arena_alloc_tab[i];

            printf("  arena leak at %p created at ", alloc.key);
            hhg_mem_alloc_loc_print(alloc.value);
            putchar('\n');
        }
    }

    hmfree(mem_arena_alloc_tab);

    printf(
        "memory allocation summary:"                                                                        "\n"
        "|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|" "\n"
        "|  malloc   |  realloc  |   free    |  strdup   |   a new   | a malloc  | a strdup  |  a free   |" "\n"
        "|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|" "\n"
        "| %9zu | %9zu | %9zu | %9zu | %9zu | %9zu | %9zu | %9zu |"                                         "\n"
        "|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|" "\n",
        mem_mallocs,
        mem_reallocs,
        mem_frees,
        mem_strdups,
        mem_arena_news,
        mem_arena_mallocs,
        mem_arena_strdups,
        mem_arena_frees
    );

    mem_debug = prev_mem_debug;
}

static void hhg_mem_alloc_tab_add(void *ptr, hhg_mem_alloc_loc_t loc)
{
    hhg_mem_alloc_loc_t *loc_arr = NULL;
    arrput(loc_arr, loc);
    hmput(mem_alloc_tab, ptr, loc_arr);
}

static void hhg_mem_alloc_loc_print(hhg_mem_alloc_loc_t loc) {
    printf("%s:%i (%s)", loc.file, loc.line, loc.func);
}

static void *hhg_mem_debug_run_func(
    void (*func)(
        void *ptr1,
        void *ptr2,
        size_t size,
        hhg_mem_alloc_loc_t loc
    ),
    void *ptr1,
    void *ptr2,
    size_t size,
    hhg_mem_alloc_loc_t loc,
    const char *func_name,
    size_t *func_counter,
    bool is_realloc
)
{
    if (mem_debug)
        return ptr1;

    bool prev_mem_debug = mem_debug;
    mem_debug = true;

    // truncate file path (affects downstream printing)
    loc.file = fs_basename(loc.file);
    
    printf("%s: ", func_name);

    if (size == 0)
        printf("%p", ptr1);
    else if (is_realloc)
        printf("%p -> %p (%zu bytes)", ptr2, ptr1, size);
    else
        printf("%p (%zu bytes)", ptr1, size);

    fputs(" at ", stdout);
    hhg_mem_alloc_loc_print(loc);
    putchar('\n');

    func(ptr1, ptr2, size, loc);

    (*func_counter)++;

    mem_debug = prev_mem_debug;

    return ptr1;
}

static void hhg_debug_malloc_core(
    void *ptr,
    void *null,
    size_t size,
    hhg_mem_alloc_loc_t loc
)
{
    HHG_UNUSED(null, size);
    hhg_mem_alloc_tab_add(ptr, loc);
}

static void hhg_debug_realloc_core(
    void *next,
    void *ptr,
    size_t size,
    hhg_mem_alloc_loc_t loc
)
{
    HHG_UNUSED(size);

    if (ptr != next) {
        if (ptr == NULL) {
            // equivalent to malloc for new pointer
            hhg_debug_malloc_core(next, NULL, size, loc);
        } else {
            // transfer loc array to new pointer
            hhg_mem_alloc_t *prev_entry = hmgetp_null(mem_alloc_tab, ptr);
            // hhg_realloc should catch this but just in case
            hhg_assert(prev_entry != NULL);

            hhg_mem_alloc_loc_t *locs = prev_entry->value;

            arrput(locs, loc);

            hmput(mem_alloc_tab, next, locs);

            // delete previous entry
            hmdel(mem_alloc_tab, ptr);
        }
    } else {
        // add new loc to existing pointer
        hhg_mem_alloc_loc_t *loc_arr = hmget(mem_alloc_tab, ptr);
        arrput(loc_arr, loc);
    }
}

static void hhg_debug_free_core(
    void *ptr,
    void *null,
    size_t size,
    hhg_mem_alloc_loc_t loc
)
{
    HHG_UNUSED(null, size, loc);
    // can free NULL
    if (ptr == NULL)
        return;
    hhg_mem_alloc_t *entry = hmgetp_null(mem_alloc_tab, ptr);
    // hhg_free should catch this but just in case
    hhg_assert(entry != NULL);
    
    arrfree(entry->value);

    hmdel(mem_alloc_tab, ptr);
}

static void hhg_debug_strdup_core(
    void *buf,
    void *null,
    size_t size,
    hhg_mem_alloc_loc_t loc
)
{
    hhg_debug_malloc_core(buf, null, size, loc);
}

static void hhg_debug_arena_new_core(
    void *arena,
    void *null,
    size_t size,
    hhg_mem_alloc_loc_t loc
)
{
    HHG_UNUSED(null, size, loc);
    hmput(mem_arena_alloc_tab, (hhg_arena_t *)arena, loc);
}

static void hhg_debug_arena_malloc_core(
    void *ptr,
    void *null,
    size_t size,
    hhg_mem_alloc_loc_t loc
)
{
    HHG_UNUSED(ptr, null, size, loc);
}

static void hhg_debug_arena_strdup_core(
    void *buf,
    void *null,
    size_t size,
    hhg_mem_alloc_loc_t loc
)
{
    HHG_UNUSED(buf, null, size, loc);
}

static void hhg_debug_arena_free_core(
    void *arena,
    void *null,
    size_t size,
    hhg_mem_alloc_loc_t loc
)
{
    HHG_UNUSED(null, size, loc);
    hmdel(mem_arena_alloc_tab, (hhg_arena_t *)arena);
}

#endif
