/*
The MIT License (MIT)

Copyright (c) 2015 Jeff Larson

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef ARENA_H
#define ARENA_H
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#ifndef ARENA_API
#define ARENA_API
#endif

typedef struct _arena_t {
    uint8_t *region;
    size_t size;
    size_t current;
    struct _arena_t *next;
} arena_t;

ARENA_API
arena_t *arena_create();

ARENA_API
void *arena_malloc(arena_t *arena, size_t size);

ARENA_API
void arena_destroy(arena_t *arena);

#ifdef ARENA_IMPLEMENTATION

#define ARENA_PAGE_SIZE 4095
#define ARENA_ALIGN_TO(x, align) (((x) + ((align)-1)) & ~((align)-1))

typedef union _arena_max_align_t {
    long double ld;
    double d;
    void *p;
    void (*fp)(void);
    long long ll;
} arena_max_align_t;

static
arena_t *arena_create_size(size_t size)
{
    arena_t *arena = (arena_t *)malloc(sizeof(arena_t));
    if (!arena)
        return NULL;

    arena->region = (uint8_t *)malloc(size);
    arena->size = size;
    arena->current = 0;
    arena->next = NULL;

    if (!arena->region) {
        free(arena);
        return NULL;
    }
    return arena;
}

ARENA_API
arena_t *arena_create()
{
    return arena_create_size(ARENA_PAGE_SIZE);
}

static
void *arena_malloc_aligned(arena_t *arena, size_t size, size_t align)
{
    arena_t *last = arena;

    assert(align && (align & (align - 1)) == 0); // align must be power of two
    size = ARENA_ALIGN_TO(size, align);

    do {
        if ((arena->size - arena->current) >= size) {
            arena->current += size;
            return arena->region + (arena->current - size);
        }

        last = arena;
    } while ((arena = arena->next) != NULL);

    size_t asize = size > ARENA_PAGE_SIZE ? size : ARENA_PAGE_SIZE;
    arena_t *next = arena_create_size(asize);

    last->next = next;
    next->current += size;
    return next->region;
}

ARENA_API
void *arena_malloc(arena_t *arena, size_t size)
{   
    return arena_malloc_aligned(arena, size, sizeof(arena_max_align_t));
}

ARENA_API
void arena_destroy(arena_t *arena)
{
    arena_t *next, *last = arena;
    do {
        next = last->next;
        free(last->region);
        free(last);
        last = next;
    } while (next != NULL);
}

#endif

#endif