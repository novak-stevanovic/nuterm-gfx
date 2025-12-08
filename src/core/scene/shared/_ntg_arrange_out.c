#include <assert.h>

#include "core/scene/shared/_ntg_arrange_out.h"
#include "core/scene/shared/_ntg_arrange_ctx.h"
#include "core/scene/shared/_ntg_map.h"
#include "shared/sarena.h"

struct ntg_arrange_out
{
    struct ntg_map __base;
};

ntg_arrange_out* ntg_arrange_out_new(size_t capacity, sarena* arena)
{
    assert(arena != NULL);

    sa_err _err;

    ntg_arrange_out* new = (ntg_arrange_out*)sarena_malloc(arena,
            sizeof(ntg_arrange_out), &_err);
    assert(new != NULL);

    __ntg_map_init__(&new->__base, capacity, sizeof(struct ntg_arrange_result), arena);

    return new;
}

struct ntg_arrange_result ntg_arrange_out_get(
        const ntg_arrange_out* out,
        const ntg_drawable* child)
{
    assert(out != NULL);
    assert(child != NULL);

    void* result = ntg_map_get(&out->__base, child); 
    if(result == NULL) return (struct ntg_arrange_result) {0};
    else return *(struct ntg_arrange_result*)result;
}

void ntg_arrange_out_set(ntg_arrange_out* out,
        const ntg_drawable* child,
        struct ntg_arrange_result result)
{
    assert(out != NULL);
    assert(child != NULL);

    ntg_map_set(&out->__base, child, &result);
}
