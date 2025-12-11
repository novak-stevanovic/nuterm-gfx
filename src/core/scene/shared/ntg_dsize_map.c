#include <assert.h>
#include "core/scene/shared/ntg_dsize_map.h"
#include "core/scene/shared/_ntg_map.h"
#include "shared/sarena.h"

struct ntg_dsize_map
{
    struct ntg_map __base;
};

ntg_dsize_map* ntg_dsize_map_new(size_t capacity, sarena* arena)
{
    assert(arena != NULL);

    sa_err _err;

    ntg_dsize_map* new = (ntg_dsize_map*)sarena_malloc(arena,
            sizeof(ntg_dsize_map), &_err);
    assert(new != NULL);

    __ntg_map_init__(&new->__base, capacity, sizeof(size_t), arena);

    return new;
}

size_t ntg_dsize_map_get(
        const ntg_dsize_map* ctx,
        const ntg_drawable* child)
{
    assert(ctx != NULL);
    assert(child != NULL);

    void* data = ntg_map_get(&ctx->__base, child); 
    if(data == NULL) return (size_t) {0};
    else return *(size_t*)data;
}

void ntg_dsize_map_set(ntg_dsize_map* ctx,
        const ntg_drawable* child,
        size_t data)
{
    assert(ctx != NULL);
    assert(child != NULL);

    ntg_map_set(&ctx->__base, child, &data);
}
