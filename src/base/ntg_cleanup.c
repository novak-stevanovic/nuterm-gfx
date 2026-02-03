#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include "ntg.h"

// static int cleanup_pair_cmp_fn(void* _p1, void* _p2)
// {
//     struct ntg_cleanup_pair* p1 = _p1;
//     struct ntg_cleanup_pair* p2 = _p2;
// 
//     return ((p1->object == p2->object) && (p1.
// }

// cmp_fn is not needed because no cmps will be done
GENC_VECTOR_GENERATE(cleanup_pair_vec, struct ntg_cleanup_pair, 1.5, NULL);

struct ntg_cleanup_batch
{
    struct cleanup_pair_vec vec;
};

ntg_cleanup_batch* ntg_cleanup_batch_new()
{
    ntg_cleanup_batch* new = malloc(sizeof(struct ntg_cleanup_batch));
    assert(new);

    cleanup_pair_vec_init(&new->vec, 10, NULL);
    return new;
}

void ntg_cleanup_batch_destroy(ntg_cleanup_batch* batch)
{
    assert(batch);
    
    size_t i;
    for(i = 0; i < batch->vec.size; i++)
    {
        batch->vec.data[i].cleanup_fn(batch->vec.data[i].object);
    }

    cleanup_pair_vec_deinit(&batch->vec, NULL);

    free(batch);
}

void ntg_cleanup_batch_add(ntg_cleanup_batch* batch, struct ntg_cleanup_pair pair)
{
    assert(batch);
    assert(pair.cleanup_fn);

    cleanup_pair_vec_pushb(&batch->vec, pair, NULL);
}

void ntg_cleanup_batch_add_array(
        ntg_cleanup_batch* batch,
        const struct ntg_cleanup_pair* pairs,
        size_t count)
{
    assert(batch);
    if(count == 0) return;

    size_t i;
    for(i = 0; i < count; i++)
    {
        ntg_cleanup_batch_add(batch, pairs[i]);
    }
}
