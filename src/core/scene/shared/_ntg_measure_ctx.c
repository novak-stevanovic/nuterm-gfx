#include <assert.h>

#include "core/scene/shared/_ntg_measure_ctx.h"
#include "core/scene/shared/_ntg_map.h"
#include "shared/sarena.h"

struct ntg_measure_ctx
{
    struct ntg_map __base;
};

ntg_measure_ctx* ntg_measure_ctx_new(size_t capacity, sarena* arena)
{
    assert(arena != NULL);

    sa_err _err;

    ntg_measure_ctx* new = (ntg_measure_ctx*)sarena_malloc(arena,
            sizeof(ntg_measure_ctx), &_err);
    assert(new != NULL);

    __ntg_map_init__(&new->__base, capacity, sizeof(struct ntg_measure_data), arena);

    return new;
}

struct ntg_measure_data ntg_measure_ctx_get(
        const ntg_measure_ctx* ctx,
        const ntg_drawable* child)
{
    assert(ctx != NULL);
    assert(child != NULL);

    void* data = ntg_map_get(&ctx->__base, child); 
    if(data == NULL) return (struct ntg_measure_data) {0};
    else return *(struct ntg_measure_data*)data;
}

void ntg_measure_ctx_set(ntg_measure_ctx* ctx,
        const ntg_drawable* child,
        struct ntg_measure_data data)
{
    assert(ctx != NULL);
    assert(child != NULL);

    ntg_map_set(&ctx->__base, child, &data);
}
