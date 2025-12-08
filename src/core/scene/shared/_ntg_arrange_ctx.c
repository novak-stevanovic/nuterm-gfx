#include <assert.h>

#include "core/scene/shared/_ntg_arrange_ctx.h"
#include "core/scene/shared/_ntg_map.h"
#include "shared/sarena.h"

struct ntg_arrange_ctx
{
    struct ntg_map __base;
};

ntg_arrange_ctx* ntg_arrange_ctx_new(size_t capacity, sarena* arena)
{
    assert(arena != NULL);

    sa_err _err;

    ntg_arrange_ctx* new = (ntg_arrange_ctx*)sarena_malloc(arena,
            sizeof(ntg_arrange_ctx), &_err);
    assert(new != NULL);

    __ntg_map_init__(&new->__base, capacity, sizeof(struct ntg_arrange_data), arena);

    return new;
}

struct ntg_arrange_data ntg_arrange_ctx_get(
        const ntg_arrange_ctx* ctx,
        const ntg_drawable* child)
{
    assert(ctx != NULL);
    assert(child != NULL);

    void* data = ntg_map_get(&ctx->__base, child); 
    if(data == NULL) return (struct ntg_arrange_data) {0};
    else return *(struct ntg_arrange_data*)data;
}

void ntg_arrange_ctx_set(ntg_arrange_ctx* ctx,
        const ntg_drawable* child,
        struct ntg_arrange_data data)
{
    assert(ctx != NULL);
    assert(child != NULL);

    ntg_map_set(&ctx->__base, child, &data);
}
