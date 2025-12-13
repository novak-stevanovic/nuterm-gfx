#include "shared/ntg_allocator.h"
#include <assert.h>

void _ntg_allocator_init_(ntg_allocator* allocator,
        void* (*malloc_fn)(size_t size),
        void  (*free_fn)(void* ptr),
        void* (*calloc_fn)(size_t nmemb, size_t size),
        void* (*realloc_fn)(void* ptr, size_t size))
{
    assert(allocator != NULL);
    assert(malloc_fn != NULL);
    assert(realloc_fn != NULL);
    assert(calloc_fn != NULL);

    allocator->__malloc_fn = malloc_fn;
    allocator->__realloc_fn = realloc_fn;
    allocator->__calloc_fn = calloc_fn;
    allocator->__free_fn = free_fn;
}

void _ntg_allocator_deinit_(ntg_allocator* allocator)
{
    assert(allocator != NULL);

    (*allocator) = (ntg_allocator) {0};
}

void* ntg_allocator_malloc(ntg_allocator* allocator, size_t size)
{
    assert(allocator != NULL);

    return allocator->__malloc_fn(size);
}

void  ntg_allocator_free(ntg_allocator* allocator, void* ptr)
{
    assert(allocator != NULL);

    return allocator->__free_fn(ptr);
}

void* ntg_allocator_calloc(ntg_allocator* allocator, size_t nmemb, size_t size)
{
    assert(allocator != NULL);

    return allocator->__calloc_fn(nmemb, size);
}

void* ntg_allocator_realloc(ntg_allocator* allocator, void* ptr, size_t size)
{
    assert(allocator != NULL);

    return allocator->__realloc_fn(ptr, size);
}
