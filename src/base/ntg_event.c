#include "ntg.h"
#include "shared/ntg_shared_internal.h"
#include <stdlib.h>

GENC_VECTOR_GENERATE(ntg_event_binding_vec, ntg_event_binding*, 1.3, NULL);

struct ntg_event_delegate
{
    struct ntg_event_binding_vec bindings;
};

struct ntg_event_binding
{
    ntg_event_delegate* delegate;
    void* subscriber;
    void (*handler_fn)(void* subscriber, struct ntg_event event);
};

ntg_event_delegate* ntg_event_delegate_new()
{
    ntg_event_delegate* new = malloc(sizeof(ntg_event_delegate));
    if(!new)
        return NULL;

    int status;
    ntg_event_binding_vec_init(&new->bindings, 3, &status);
    if(status != GENC_SUCCESS)
    {
        free(new);
        return NULL;
    }

    return new;
}

void ntg_event_delegate_destroy(ntg_event_delegate* delegate)
{
    if(!delegate)
        return;

    size_t i;
    for(i = 0; i < delegate->bindings.size; i++)
    {
        delegate->bindings.data[i]->delegate = NULL;
    }

    ntg_event_binding_vec_deinit(&delegate->bindings, NULL);
    free(delegate);
}

ntg_event_binding* ntg_event_bind(
        ntg_event_delegate* delegate,
        void* subscriber,
        void (*handler_fn)(void* subscriber, struct ntg_event event),
        int* out_status)
{
    ntg_init_status(out_status);

    if(!delegate || !handler_fn)
        ntg_return(NULL, out_status, NTG_ERR_INVALID_ARG);

    ntg_event_binding* new = malloc(sizeof(ntg_event_binding));
    if(!new)
        ntg_return(NULL, out_status, NTG_ERR_ALLOC_FAIL);

    new->delegate = delegate;
    new->subscriber = subscriber;
    new->handler_fn = handler_fn;

    int _status;
    ntg_event_binding_vec_pushb(&delegate->bindings, new, &_status);
    if(_status != GENC_SUCCESS)
    {
        free(new);

        switch(_status)
        {
            case GENC_ERR_ALLOC_FAIL:
                ntg_return(NULL, out_status, NTG_ERR_ALLOC_FAIL);

            default:
                ntg_return(NULL, out_status, NTG_ERR_UNEXPECTED);
        }
    }

    return new;
}

void ntg_event_raise(ntg_event_delegate* delegate, struct ntg_event event)
{
    if(!delegate)
        return;
    
    size_t i;
    ntg_event_binding* it_binding;
    for(i = 0; i < delegate->bindings.size; i++)
    {
        it_binding = delegate->bindings.data[i];
        it_binding->handler_fn(it_binding->subscriber, event);
    }
}

void ntg_event_unbind(ntg_event_binding* binding)
{
    if(!binding)
        return;

    if(binding->delegate)
    {
        ntg_event_binding_vec_rm(&binding->delegate->bindings, binding, NULL);
    }

    free(binding);
}
