#include <assert.h>
#include "core/scene/shared/ntg_dmeasure_map.h"
#include "core/scene/shared/_ntg_map.h"
#include "shared/sarena.h"

struct ntg_dmeasure_map
{
    struct ntg_map __base;
};

ntg_dmeasure_map* ntg_dmeasure_map_new(size_t capacity, sarena* arena)
{
    assert(arena != NULL);

    sa_err _err;

    ntg_dmeasure_map* new = (ntg_dmeasure_map*)sarena_malloc(arena,
            sizeof(ntg_dmeasure_map), &_err);
    assert(new != NULL);

    __ntg_map_init__(&new->__base, capacity, sizeof(struct ntg_measurement), arena);

    return new;
}

struct ntg_measurement ntg_dmeasure_map_get(
        const ntg_dmeasure_map* ctx,
        const ntg_drawable* child)
{
    assert(ctx != NULL);
    assert(child != NULL);

    void* data = ntg_map_get(&ctx->__base, child); 
    if(data == NULL) return (struct ntg_measurement) {0};
    else return *(struct ntg_measurement*)data;
}

void ntg_dmeasure_map_set(ntg_dmeasure_map* ctx,
        const ntg_drawable* child,
        struct ntg_measurement data)
{
    assert(ctx != NULL);
    assert(child != NULL);

    ntg_map_set(&ctx->__base, child, &data);
}
