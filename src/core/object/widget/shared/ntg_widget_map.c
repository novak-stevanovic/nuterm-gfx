#include "ntg.h"
#include <assert.h>
#include "core/object/widget/shared/ntg_widget_map.h"

void ntg_widget_map_init(ntg_widget_map* ctx, size_t capacity, size_t data_size,
                         sarena* arena)
{
    assert(ctx != NULL);
    assert(data_size > 0);
    assert(arena != NULL);

    ctx->__count = 0;
    ctx->__data_size = data_size;
    ctx->__capacity = capacity;

    if(capacity > 0)
    {
        ctx->__keys = (const ntg_widget**)sarena_calloc(arena, capacity * sizeof(void*));
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

void ntg_widget_map_set(ntg_widget_map* ctx, const ntg_widget* widget,
                        void* data)
{
    assert(ctx != NULL);
    assert(widget != NULL);
    assert(ctx->__keys != NULL);

    size_t i;
    for(i = 0; i < ctx->__count; i++)
    {
        if(ctx->__keys[i] == widget) break;
    }

    if(i < ctx->__count)
    {
        memcpy(ctx->__values + (i * ctx->__data_size), data, ctx->__data_size);
    }
    else
    {
        assert(ctx->__count < ctx->__capacity);

        ctx->__keys[ctx->__count] = widget;

        memcpy(ctx->__values + (ctx->__data_size * ctx->__count),
                data,
                ctx->__data_size);
        
        ctx->__count += 1;
    }

}

void* ntg_widget_map_get(const ntg_widget_map* ctx, const ntg_widget* widget)
{
    assert(ctx != NULL);
    assert(widget != NULL);
    assert(ctx->__keys != NULL);

    size_t i;
    for(i = 0; i < ctx->__count; i++)
    {
        if(ctx->__keys[i] == widget)
            return (ctx->__values + (i * ctx->__data_size));
    }

    return NULL;
}
