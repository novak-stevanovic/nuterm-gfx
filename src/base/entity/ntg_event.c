#include "ntg.h"
#include "thirdparty/genc.h"

struct ntg__event_sub
{
    ntg_event_binding* binding;
    ntg_entity* subscriber;
    void (*handler_fn)(ntg_entity* subscriber, struct ntg_event event);

    bool removed;
};

GENC_VECTOR_DEFINE(ntg__event_sub_vec, struct ntg__event_sub, 1.5, )

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg_event_delegate_init(ntg_event_delegate* delegate)
{
    if(!delegate) return NTG_ERR_INV_ARG;

    (*delegate) = (struct ntg_event_delegate) {0};

    return 0;
}

int ntg_event_delegate_deinit(ntg_event_delegate* delegate)
{
    if(!delegate) return NTG_ERR_INV_ARG;
    if(delegate->priv.raise) return NTG_ERR_INV_STATE;

    struct ntg__event_sub_vec* subs = &delegate->priv.subs;

    size_t i;
    for(i = 0; i < subs->size; i++)
    {
        if(subs->data[i].binding)
            subs->data[i].binding->priv.delegate = NULL;
    }

    ntg__event_sub_vec_deinit(subs);

    (*delegate) = (struct ntg_event_delegate) {0};

    return 0;
}

int ntg_event_bind(
        ntg_event_delegate* delegate,
        ntg_entity* subscriber, 
        void (*handler_fn)(ntg_entity* subscriber, struct ntg_event event),
        ntg_event_binding* out_binding)
{
    if(!delegate || !handler_fn) return NTG_ERR_INV_ARG;
    if(out_binding && out_binding->priv.delegate) return NTG_ERR_INV_ARG;

    struct ntg__event_sub sub = {
        .binding = out_binding,
        .subscriber = subscriber,
        .handler_fn = handler_fn
    };

    int status = ntg__event_sub_vec_pushb(&delegate->priv.subs, sub);
    switch(status)
    {
        case 0:
            if(out_binding) out_binding->priv.delegate = delegate;
            return 0;
        case GENC_ERR_ALLOC_FAIL:
            if(out_binding) out_binding->priv.delegate = NULL;
            return NTG_ERR_ALLOC_FAIL;
        default:
            if(out_binding) out_binding->priv.delegate = NULL;
            return NTG_ERR_UNEXPECTED;
    }
}

int ntg_event_unbind(ntg_event_binding* binding)
{
    if(!binding) return NTG_ERR_INV_ARG;

    ntg_event_delegate* delegate = binding->priv.delegate;
    if(!delegate) return 0;

    struct ntg__event_sub_vec* subs = &delegate->priv.subs;

    size_t i;
    int status;
    for(i = 0; i < subs->size; i++)
    {
        if(subs->data[i].binding == binding)
        {
            if(delegate->priv.raise) // Defer if inside `ntg_event_raise()`
            {
                subs->data[i].removed = true;
                return 0;
            }
            else
            {
                status = ntg__event_sub_vec_rm_at(subs, i);
                switch(status)
                {
                    case 0:
                        binding->priv.delegate = NULL;
                        return 0;
                    default:
                        return NTG_ERR_UNEXPECTED;
                }
            }
        }
    }

    if(i >= subs->size)
        return NTG_ERR_UNEXPECTED;

    return 0;
}

int ntg_event_raise(ntg_event_delegate* delegate, struct ntg_event event)
{
    if(!delegate || event.type == NTG_EVENT_INVALID) return NTG_ERR_INV_ARG;
    if(delegate->priv.raise) return NTG_ERR_INV_STATE;

    struct ntg__event_sub_vec* subs = &delegate->priv.subs;
    struct ntg__event_sub* it_sub;

    delegate->priv.raise = true;

    /* Save size in case any handlers use `ntg_event_bind()` and increase the
     * vector size. */
    size_t size = subs->size;

    size_t i;
    for(i = 0; i < size; i++)
    {
        it_sub = &subs->data[i];

        if(it_sub->handler_fn && !it_sub->removed)
            it_sub->handler_fn(it_sub->subscriber, event);
    }

    delegate->priv.raise = false;

    i = subs->size;
    while(i > 0)
    {
        i--;

        it_sub = &subs->data[i];

        if(it_sub->removed)
        {
            it_sub->binding->priv.delegate = NULL; 

            /* Must not fail */
            ntg__event_sub_vec_rm_at(subs, i);
        }
    }

    return 0;
}
