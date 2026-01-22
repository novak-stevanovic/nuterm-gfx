#include "ntg.h"
#include <assert.h>
#include "core/object/shared/ntg_object_map.h"

void ntg_object_map_init(ntg_object_map* ctx, size_t capacity,
        size_t data_size, sarena* arena)
{
    assert(ctx != NULL);
    assert(data_size > 0);
    assert(arena != NULL);

    ctx->__count = 0;
    ctx->__data_size = data_size;
    ctx->__capacity = capacity;

    if(capacity > 0)
    {
        ctx->__keys = (const ntg_object**)sarena_calloc(arena, capacity * sizeof(void*));
        assert(ctx->__keys != NULL);

        ctx->__values = (char*)sarena_calloc(arena, capacity * data_size);
        assert(ctx->__values != NULL);
    }
    else
    {
        ctx->__keys = NULL;
        ctx->__values = NULL;
    }
}

void ntg_object_map_set(ntg_object_map* ctx, const ntg_object* object,
                        void* data)
{
    assert(ctx != NULL);
    assert(object != NULL);
    assert(ctx->__keys != NULL);

    size_t i;
    for(i = 0; i < ctx->__count; i++)
    {
        if(ctx->__keys[i] == object) break;
    }

    if(i < ctx->__count)
    {
        memcpy(ctx->__values + (i * ctx->__data_size), data, ctx->__data_size);
    }
    else
    {
        assert(ctx->__count < ctx->__capacity);

        ctx->__keys[ctx->__count] = object;

        memcpy(ctx->__values + (ctx->__data_size * ctx->__count),
                data,
                ctx->__data_size);
        
        ctx->__count += 1;
    }

}

void* ntg_object_map_get(const ntg_object_map* ctx, const ntg_object* object)
{
    assert(ctx != NULL);
    assert(object != NULL);
    assert(ctx->__keys != NULL);

    size_t i;
    for(i = 0; i < ctx->__count; i++)
    {
        if(ctx->__keys[i] == object)
            return (ctx->__values + (i * ctx->__data_size));
    }

    return NULL;
}
