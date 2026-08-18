#include "ntg.h"
#include <stdlib.h>
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_cleanup_data
{
    void* data;
    void (*deinit_fn)(void* object);
    void (*free_fn)(void* object);
};

GENC_VECTOR_GENERATE(ntg_cleanup_data_vec, struct ntg_cleanup_data, 1.5)

struct ntg_cleanup_batch
{
    struct ntg_cleanup_data_vec vec;

    bool deinit, free;
};

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* CLEANUP BATCH */
/* ------------------------------------------------------ */

ntg_cleanup_batch* ntg_cleanup_batch_new(void)
{
    ntg_cleanup_batch* new = malloc(sizeof(struct ntg_cleanup_batch));
    if(!new) return NULL;

    new->deinit = false;
    new->free = false;

    new->vec = (struct ntg_cleanup_data_vec) {0};
    if(ntg_cleanup_data_vec_prealloc(&new->vec, 20) != 0)
    {
        free(new);
        return NULL;
    }

    return new;
}

void ntg_cleanup_batch_finish(ntg_cleanup_batch* batch)
{
    if(!batch) return;

    if(batch->deinit) return;

    size_t i;
    struct ntg_cleanup_data it_data;
    for(i = 0; i < ntg_cleanup_data_vec_size(&batch->vec); i++)
    {
        it_data = ntg_cleanup_data_vec_data(&batch->vec)[i];

        if(it_data.deinit_fn)
            it_data.deinit_fn(it_data.data);

        if(it_data.free_fn)
            it_data.free_fn(it_data.data);
    }
    
    (void)ntg_cleanup_data_vec_deinit(&batch->vec);

    free(batch);
}

void ntg_cleanup_batch_add(
        ntg_cleanup_batch* batch,
        void* data,
        void (*deinit_fn)(void* data),
        void (*free_fn)(void* data),
        int* out_status)
{
    ntg_init_status(out_status);

    if(!batch)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    struct ntg_cleanup_data cleanup_data = {
        .data = data,
        .deinit_fn = deinit_fn,
        .free_fn = free_fn
    };

    int _status;
    _status = ntg_cleanup_data_vec_pushb(&batch->vec, cleanup_data);

    if(_status != 0)
    {
        switch(_status)
        {
            case GENC_ERR_ALLOC_FAIL:
                ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);

            default:
                ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
        }
    }
}
