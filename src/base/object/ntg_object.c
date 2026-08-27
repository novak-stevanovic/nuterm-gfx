#include "ntg.h"
#include "shared/ntg_shared_internal.h"

int ntg_object_vdeinit(ntg_object* object)
{
    if(!object) return NTG_ERR_INV_ARG;

    if(object->ro.vtable && object->ro.vtable->deinit_fn)
        object->ro.vtable->deinit_fn(object);

    return 0;
}

void ntg_object_vdeinit_void(void* _object)
{
    if(!_object) return;

    ntg_object_vdeinit(_object);
}

int ntg_object_event_raise(ntg_object* object, unsigned int event_type, void* event_data)
{
    if(!object)
        return NTG_ERR_INV_ARG;

    return ntg_event_raise(
        &object->ro.event_dlgt,
        ntg_event_new(event_type, object, event_data));
}

int ntg_object_event_bind(
        ntg_object* observed,
        ntg_object* observer,
        void (*handler_fn)(ntg_object* observer, struct ntg_event event),
        ntg_event_binding* out_binding)
{
    if(!observed)
        return NTG_ERR_INV_ARG;

    return ntg_event_bind(&observed->ro.event_dlgt, observer, handler_fn, out_binding);

}

int ntg_object_event_unbind(ntg_event_binding* out_binding)
{
    return ntg_event_unbind(out_binding);
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

int ntg_object_init_inherit(
        ntg_object* object,
        const struct ntg_object_vtable* vtable,
        const ntg_type* type)
{
    if(!object || !type || !vtable)
        return NTG_ERR_INV_ARG;

    if(!ntg_type_instanceof(type, &NTG_TYPE_OBJECT))
        return NTG_ERR_BAD_TYPE;

    if(!vtable->deinit_fn)
        return NTG_ERR_BAD_VTABLE;

    (*object) = (ntg_object) {0};

    int status = ntg_event_delegate_init(&object->ro.event_dlgt);
    switch(status)
    {
        case 0:
            break;
        default:
            return NTG_ERR_UNEXPECTED;
    }

    object->ro.vtable = vtable;
    object->ro.type = type;

    return 0;
}

int ntg_object_deinit(ntg_object* object)
{
    if(!object) return NTG_ERR_UNEXPECTED;

    ntg_event_delegate_deinit(&object->ro.event_dlgt);

    (*object) = (ntg_object) {0};

    return 0;
}
