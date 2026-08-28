#ifndef NTG_OBJECT_H
#define NTG_OBJECT_H

#include "shared/ntg_shared.h"
#include "base/object/ntg_event.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

struct ntg_object_vtable
{
    void (*deinit_fn)(ntg_object* object);
};

struct ntg_object
{
    struct
    {
        const char* name; /* Can be used for logging */
    } pub;

    struct
    {
        const struct ntg_object_vtable* vtable;
        const ntg_type* type;
        ntg_event_delegate event_dlgt;
    } ro;
};

NTG_API int
ntg_object_vdeinit(ntg_object* object);

NTG_API void
ntg_object_vdeinit_void(void* _object);

NTG_API int
ntg_object_event_raise(ntg_object* object, unsigned int event_type, void* event_data);

NTG_API int
ntg_object_event_bind(
        ntg_object* observed,
        ntg_object* observer,
        void (*handler_fn)(ntg_object* observer, struct ntg_event event),
        ntg_event_binding* out_binding);

NTG_API int
ntg_object_event_unbind(ntg_event_binding* out_binding);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

NTG_API int
ntg_object_init_inherit(
        ntg_object* object,
        const struct ntg_object_vtable* vtable,
        const ntg_type* type);

int ntg_object_deinit(ntg_object* object);

#endif // NTG_OBJECT_H
