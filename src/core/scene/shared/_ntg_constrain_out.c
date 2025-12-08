#include <assert.h>

#include "core/scene/shared/_ntg_constrain_out.h"
#include "core/scene/shared/_ntg_map.h"
#include "shared/sarena.h"

struct ntg_constrain_out
{
    struct ntg_map __base;
};

ntg_constrain_out* ntg_constrain_out_new(size_t capacity, sarena* arena)
{
    assert(arena != NULL);

    sa_err _err;

    ntg_constrain_out* new = (ntg_constrain_out*)sarena_malloc(arena,
            sizeof(ntg_constrain_out), &_err);
    assert(new != NULL);

    __ntg_map_init__(&new->__base, capacity, sizeof(struct ntg_constrain_result), arena);

    return new;
}

struct ntg_constrain_result ntg_constrain_out_get(
        const ntg_constrain_out* out,
        const ntg_drawable* child)
{
    assert(out != NULL);
    assert(child != NULL);

    void* result = ntg_map_get(&out->__base, child); 
    if(result == NULL) return (struct ntg_constrain_result) {0};
    else return *(struct ntg_constrain_result*)result;
}

void ntg_constrain_out_set(ntg_constrain_out* out,
        const ntg_drawable* child,
        struct ntg_constrain_result result)
{
    assert(out != NULL);
    assert(child != NULL);

    ntg_map_set(&out->__base, child, &result);
}
