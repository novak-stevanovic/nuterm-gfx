#include "ntg.h"
#include <stdlib.h>
#include <assert.h>

struct ntg_cleanup_data
{
    void* data;
    void (*deinit_fn)(void* object);
    void (*free_fn)(void* object);
};

GENC_VECTOR_GENERATE(ntg_cleanup_data_vec, struct ntg_cleanup_data, 1.5, NULL);

struct ntg_cleanup_batch
{
    struct ntg_cleanup_data_vec vec;

    bool deinit, free;
};

ntg_cleanup_batch* ntg_cleanup_batch_new()
{
    ntg_cleanup_batch* new = malloc(sizeof(struct ntg_cleanup_batch));
    assert(new);
    new->deinit = false;
    new->free = false;

    ntg_cleanup_data_vec_init(&new->vec, 20, NULL);

    return new;
}

void ntg_cleanup_batch_finish(ntg_cleanup_batch* batch)
{
    assert(batch);

    if(batch->deinit) return;

    size_t i;
    struct ntg_cleanup_data it_data;
    for(i = 0; i < batch->vec.size; i++)
    {
        it_data = batch->vec.data[i];

        if(it_data.deinit_fn)
            it_data.deinit_fn(it_data.data);

        if(it_data.free_fn)
            it_data.free_fn(it_data.data);
    }
    
    ntg_cleanup_data_vec_deinit(&batch->vec, NULL);

    free(batch);
}

void ntg_cleanup_batch_add(
        ntg_cleanup_batch* batch,
        void* data,
        void (*deinit_fn)(void* data),
        void (*free_fn)(void* data))
{
    assert(batch);

    struct ntg_cleanup_data cleanup_data = {
        .data = data,
        .deinit_fn = deinit_fn,
        .free_fn = free_fn
    };

    ntg_cleanup_data_vec_pushb(&batch->vec, cleanup_data, NULL);
}
