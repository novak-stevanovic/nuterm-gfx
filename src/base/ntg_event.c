#include "ntg.h"
#include "shared/ntg_shared_internal.h"
#include <assert.h>
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
    assert(new);

    ntg_event_binding_vec_init(&new->bindings, 3, NULL);
    return new;
}

void ntg_event_delegate_destroy(ntg_event_delegate* delegate)
{
    assert(delegate);

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
        void (*handler_fn)(void* subscriber, struct ntg_event event))
{
    assert(delegate);
    if(!handler_fn) return NULL;

    ntg_event_binding* new = malloc(sizeof(ntg_event_binding));
    new->delegate = delegate;
    new->subscriber = subscriber;
    new->handler_fn = handler_fn;

    ntg_event_binding_vec_pushb(&delegate->bindings, new, NULL);

    return new;
}

void ntg_event_raise(ntg_event_delegate* delegate, struct ntg_event event)
{
    assert(delegate);
    
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
    assert(binding);

    if(binding->delegate)
    {
        ntg_event_binding_vec_rm(&binding->delegate->bindings, binding, NULL);
    }

    free(binding);
}
