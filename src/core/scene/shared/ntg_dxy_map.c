#include <assert.h>
#include "core/scene/shared/ntg_dxy_map.h"
#include "core/scene/shared/_ntg_map.h"
#include "shared/sarena.h"

struct ntg_dxy_map
{
    struct ntg_map __base;
};

ntg_dxy_map* ntg_dxy_map_new(size_t capacity, sarena* arena)
{
    assert(arena != NULL);

    sa_err _err;

    ntg_dxy_map* new = (ntg_dxy_map*)sarena_malloc(arena,
            sizeof(ntg_dxy_map), &_err);
    assert(new != NULL);

    __ntg_map_init__(&new->__base, capacity, sizeof(struct ntg_xy), arena);

    return new;
}

struct ntg_xy ntg_dxy_map_get(
        const ntg_dxy_map* ctx,
        const ntg_drawable* child)
{
    assert(ctx != NULL);
    assert(child != NULL);

    void* data = ntg_map_get(&ctx->__base, child); 
    if(data == NULL) return (struct ntg_xy) {0};
    else return *(struct ntg_xy*)data;
}

void ntg_dxy_map_set(ntg_dxy_map* ctx,
        const ntg_drawable* child,
        struct ntg_xy data)
{
    assert(ctx != NULL);
    assert(child != NULL);

    ntg_map_set(&ctx->__base, child, &data);
}
