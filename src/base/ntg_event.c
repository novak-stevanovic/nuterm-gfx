#include "ntg.h"
#include "shared/ntg_shared_internal.h"
#include <stdlib.h>

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

GENC_VECTOR_GENERATE(ntg_event_binding_vec, ntg_event_binding*, 1.3)

static int
ntg_event_binding_vec_rm_value(
        struct ntg_event_binding_vec* vec,
        const ntg_event_binding* binding)
{
    if(!vec)
        return GENC_ERR_INVALID_ARG;

    ntg_event_binding** data = ntg_event_binding_vec_data(vec);
    size_t size = ntg_event_binding_vec_size(vec);

    size_t i;
    for(i = 0; i < size; i++)
    {
        if(data[i] == binding)
            return ntg_event_binding_vec_rm_at(vec, i);
    }

    return GENC_ERR_NO_DATA;
}

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

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

ntg_event_delegate* ntg_event_delegate_new(void)
{
    ntg_event_delegate* new = malloc(sizeof(ntg_event_delegate));
    if(!new)
        return NULL;

    new->bindings = (struct ntg_event_binding_vec) {0};
    int status = ntg_event_binding_vec_prealloc(&new->bindings, 3);
    if(status != 0)
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
    for(i = 0; i < ntg_event_binding_vec_size(&delegate->bindings); i++)
    {
        ntg_event_binding_vec_data(&delegate->bindings)[i]->delegate = NULL;
    }

    (void)ntg_event_binding_vec_deinit(&delegate->bindings);
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
    _status = ntg_event_binding_vec_pushb(&delegate->bindings, new);
    if(_status != 0)
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
    for(i = 0; i < ntg_event_binding_vec_size(&delegate->bindings); i++)
    {
        it_binding = ntg_event_binding_vec_data(&delegate->bindings)[i];
        it_binding->handler_fn(it_binding->subscriber, event);
    }
}

void ntg_event_unbind(ntg_event_binding* binding)
{
    if(!binding)
        return;

    if(binding->delegate)
    {
        (void)ntg_event_binding_vec_rm_value(&binding->delegate->bindings, binding);
    }

    free(binding);
}
