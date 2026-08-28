#include "ntg.h"
#include <stdlib.h>
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_garbage_item
{
    void* data;
    void (*deinit_fn)(void* object);
    void (*free_fn)(void* object);
};

struct ntg_garbage
{
    struct ntg_garbage_item* items;
    size_t size, cap;
};

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

ntg_garbage* ntg_garbage_new(size_t cap)
{
    if(cap == 0)
        cap = NTG_GARBAGE_CAP_AUTO;

    ntg_garbage* garbage = malloc(sizeof(ntg_garbage));
    if(!garbage) return NULL;

    garbage->items = malloc(cap * sizeof(struct ntg_garbage_item));
    if(!garbage->items)
    {
        free(garbage);
        return NULL;
    }

    garbage->size = 0;
    garbage->cap = cap;
    
    return garbage;
}

void ntg_garbage_throw(ntg_garbage* garbage)
{
    if(!garbage) return;

    size_t i;
    for(i = 0; i < garbage->size; i++)
    {
        ntg_log_log("GARBAGE COLLECT: %p", garbage->items + i);

        if(garbage->items[i].deinit_fn)
            garbage->items[i].deinit_fn(garbage->items[i].data);

        if(garbage->items[i].free_fn)
            garbage->items[i].free_fn(garbage->items[i].data);
    }

    garbage->size = 0;
}

void ntg_garbage_destroy(ntg_garbage* garbage)
{
    if(!garbage) return;

    ntg_garbage_throw(garbage);

    free(garbage->items);
    (*garbage) = (ntg_garbage) {0};
    free(garbage);
}

int ntg_garbage_add(
        ntg_garbage* garbage,
        void* data,
        void (*deinit_fn)(void* data),
        void (*free_fn)(void* data))
{
    if((!garbage) || (!deinit_fn && !free_fn))
        return NTG_ERR_INV_ARG;

    if(garbage->size >= garbage->cap)
        return NTG_ERR_NO_CAP;

    garbage->items[garbage->size] = (struct ntg_garbage_item) {
        .data = data,
        .deinit_fn = deinit_fn,
        .free_fn = free_fn
    };

    garbage->size++;

    return 0;
}
