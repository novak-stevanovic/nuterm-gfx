#include "ntg.h"
#include "shared/ntg_shared_internal"

int ntg_entity_vdeinit(ntg_entity* entity)
{
    if(!entity) return NTG_ERR_INV_ARG;

    if(entity->ro.vtable && entity->ro.vtable->deinit_fn)
        entity->ro.vtable->deinit_fn(entity);

    return 0;
}

void ntg_entity_vdeinit_void(void* _entity)
{
    if(!_entity) return;

    ntg_entity_vdeinit(_entity);
}

int ntg_entity_event_raise(ntg_entity* entity, unsigned int event_type, void* event_data)
{
    if(!entity)
        return NTG_ERR_INV_ARG;

    return ntg_event_raise(
        &entity->ro.event_dlgt,
        ntg_event_new(event_type, entity, event_data));
}

int ntg_entity_event_bind(
        ntg_entity* observed,
        ntg_entity* observer,
        void (*handler_fn)(ntg_entity* observer, struct ntg_event event),
        ntg_event_binding* out_binding)
{
    if(!observed)
        return NTG_ERR_INV_ARG;

    return ntg_event_bind(&observed->ro.event_dlgt, observer, handler_fn, out_binding);

}

int ntg_entity_event_unbind(ntg_event_binding* out_binding)
{
    return ntg_event_unbind(out_binding);
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

int ntg_entity_init_inherit(
        ntg_entity* entity,
        const struct ntg_entity_vtable* vtable,
        const ntg_type* type)
{
    if(!entity || !type || !vtable)
        return NTG_ERR_INV_ARG;

    if(!ntg_type_instanceof(type, &NTG_TYPE_ENTITY))
        return NTG_ERR_BAD_TYPE;

    if(!vtable->deinit_fn)
        return NTG_ERR_BAD_VTABLE;

    (*entity) = (ntg_entity) {0};

    int status = ntg_event_delegate_init(&entity->ro.event_dlgt);
    switch(status)
    {
        case 0:
            break;
        default:
            return NTG_ERR_UNEXPECTED;
    }

    entity->ro.vtable = vtable;
    entity->ro.type = type;

    return 0;
}

int ntg_entity_deinit(ntg_entity* entity)
{
    if(!entity) return NTG_ERR_UNEXPECTED;

    ntg_event_delegate_deinit(&entity->ro.event_dlgt);

    (*entity) = (ntg_entity) {0};
}
