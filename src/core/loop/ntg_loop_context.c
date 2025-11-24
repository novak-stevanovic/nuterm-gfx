#include <assert.h>
#include "core/loop/ntg_loop_context.h"

void __ntg_loop_context_init__(
        ntg_loop_context* context,
        ntg_stage* init_stage,
        struct ntg_xy init_app_size,
        void* data)
{
    assert(context != NULL);
    assert(init_stage != NULL);

    context->__stage = init_stage;
    context->__pending_stage = init_stage;
    context->__loop = true;
    context->__app_size = init_app_size;
    context->__data = data;
}

void __ntg_loop_context_deinit__(ntg_loop_context* context)
{
    assert(context != NULL);

    context->__stage = NULL;
    context->__pending_stage = NULL;
    context->__app_size = ntg_xy(0, 0);
    context->__loop = false;
}

void ntg_loop_context_set_stage(ntg_loop_context* context, ntg_stage* stage)
{
    assert(context != NULL);

    context->__pending_stage = stage;
}

ntg_stage* ntg_loop_context_get_stage(ntg_loop_context* context)
{
    assert(context != NULL);

    return context->__stage;
}

struct ntg_xy ntg_loop_context_get_app_size(const ntg_loop_context* context)
{
    assert(context != NULL);

    return context->__app_size;
}

void* ntg_loop_context_get_data(ntg_loop_context* context)
{
    assert(context != NULL);

    return context->__data;
}

void ntg_loop_context_exit(ntg_loop_context* context)
{
    assert(context != NULL);

    context->__loop = false;
}

/* Used by ntg_loop */
bool _ntg_loop_context_update(ntg_loop_context* context)
{
    if(context->__pending_stage != NULL)
    {
        context->__stage = context->__pending_stage;
        context->__pending_stage = NULL;
    }
    
    return context->__loop;
}

void _ntg_loop_context_set_app_size(ntg_loop_context* context, struct ntg_xy size)
{
    context->__app_size = size;
}
