#include <string.h>
#include <assert.h>

#include "core/scene/shared/_ntg_map.h"
#include "shared/_ntg_shared.h"
#include "shared/sarena.h"

void __ntg_map_init__(ntg_map* ctx, size_t capacity, size_t data_size, sarena* arena)
{
    assert(ctx != NULL);
    assert(data_size > 0);
    assert(arena != NULL);

    sa_err _err;

    ctx->__count = 0;
    ctx->__data_size = data_size;
    ctx->__capacity = capacity;

    if(capacity > 0)
    {
        ctx->__keys = (const ntg_drawable**)sarena_calloc(arena,
                capacity * sizeof(void*), &_err);
        assert(ctx->__keys != NULL);

        ctx->__values = (char*)sarena_calloc(arena,
                capacity * data_size, &_err);
        assert(ctx->__values != NULL);
    }
    else
    {
        ctx->__keys = NULL;
        ctx->__values = NULL;
    }
}

void ntg_map_set(ntg_map* ctx, const ntg_drawable* drawable, void* data)
{
    assert(ctx != NULL);
    assert(drawable != NULL);
    assert(ctx->__keys != NULL);

    size_t i;
    for(i = 0; i < ctx->__count; i++)
    {
        if(ctx->__keys[i] == drawable) break;
    }

    if(i < ctx->__count)
    {
        memcpy(ctx->__values + (i * ctx->__data_size), data, ctx->__data_size);
    }
    else
    {
        assert(ctx->__count < ctx->__capacity);

        ctx->__keys[ctx->__count] = drawable;

        memcpy(ctx->__values + (ctx->__data_size * ctx->__count),
                data,
                ctx->__data_size);
        
        ctx->__count += 1;
    }

}

void* ntg_map_get(const ntg_map* ctx, const ntg_drawable* drawable)
{
    assert(ctx != NULL);
    assert(drawable != NULL);
    assert(ctx->__keys != NULL);

    size_t i;
    for(i = 0; i < ctx->__count; i++)
    {
        if(ctx->__keys[i] == drawable)
            return (ctx->__values + (i * ctx->__data_size));
    }

    return NULL;
}
