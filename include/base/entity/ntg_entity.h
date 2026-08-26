#ifndef NTG_ENTITY_H
#define NTG_ENTITY_H

#include "shared/ntg_shared.h"
#include "base/entity/ntg_event.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

struct ntg_entity_vtable
{
    void (*deinit_fn)(ntg_entity* entity);
};

struct ntg_entity
{
    struct
    {
        const struct ntg_entity_vtable* vtable;
        const ntg_type* type;
        ntg_event_delegate event_dlgt;
    } ro;
};

NTG_API int
ntg_entity_vdeinit(ntg_entity* entity);

NTG_API void
ntg_entity_vdeinit_void(void* _entity);

NTG_API int
ntg_entity_event_raise(ntg_entity* entity, unsigned int event_type, void* event_data);

NTG_API int
ntg_entity_event_bind(
        ntg_entity* observed,
        ntg_entity* observer,
        void (*handler_fn)(ntg_entity* observer, struct ntg_event event),
        ntg_event_binding* out_binding);

NTG_API int
ntg_entity_event_unbind(ntg_event_binding* out_binding);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

NTG_API int
ntg_entity_init_inherit(
        ntg_entity* entity,
        const struct ntg_entity_vtable* vtable,
        const ntg_type* type);

int ntg_entity_deinit(ntg_entity* entity);

#endif // NTG_ENTITY_H
