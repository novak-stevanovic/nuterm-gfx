#ifndef _NTG_ALLOCATOR_H
#define _NTG_ALLOCATOR_H

#include <stddef.h>

typedef struct ntg_allocator
{
    /* read-only */
    void* (*__malloc_fn)(size_t size);
    void  (*__free_fn)(void* ptr);
    void* (*__calloc_fn)(size_t nmemb, size_t size);
    void* (*__realloc_fn)(void* ptr, size_t size);
} ntg_allocator;

void __ntg_allocator_init__(ntg_allocator* allocator,
        void* (*malloc_fn)(size_t size), /* non-NULL */
        void  (*free_fn)(void* ptr),
        void* (*calloc_fn)(size_t nmemb, size_t size), /* non-NULL */
        void* (*realloc_fn)(void* ptr, size_t size)); /* non-NULL */
void __ntg_allocator_deinit__(ntg_allocator* allocator);

void* ntg_allocator_malloc(ntg_allocator* allocator, size_t size);
void  ntg_allocator_free(ntg_allocator* allocator, void* ptr);
void* ntg_allocator_calloc(ntg_allocator* allocator, size_t nmemb, size_t size);
void* ntg_allocator_realloc(ntg_allocator* allocator, void* ptr, size_t size);

#endif // _NTG_ALLOCATOR_H
