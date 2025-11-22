#include <assert.h>
#include <stdlib.h>

#include "core/object/shared/ntg_object_fx.h"
#include "core/object/ntg_object.h"

void __ntg_object_fx_init__(
        ntg_object_fx* fx, 
        unsigned int object_type,
        ntg_object_fx_apply_fn apply_fn,
        ntg_object_fx_deinit_fn deinit_fn)
{
    assert(fx != NULL);

    fx->_object_type = object_type;
    fx->__apply_fn = apply_fn;
    fx->__deinit_fn = deinit_fn;
}

void ntg_object_fx_destroy(ntg_object_fx* fx)
{
    assert(fx != NULL);

    if(fx->__deinit_fn != NULL)
        fx->__deinit_fn(fx);

    free(fx);
}

/* Returns if the effect has been applied successfully. */
bool ntg_object_fx_apply(
        const ntg_object* object,
        const ntg_object_fx* fx,
        void* object_fx_interface)
{
    assert(object != NULL);
    assert(fx != NULL);
    assert(object_fx_interface != NULL);

    if(ntg_object_get_type(object) != fx->_object_type)
        return false;

    if(fx->__apply_fn != NULL)
        return fx->__apply_fn(object, fx, object_fx_interface);
    else
        return true;
}
