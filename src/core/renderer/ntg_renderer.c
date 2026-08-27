#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

bool ntg_renderer_render(
        ntg_renderer* renderer,
        const ntg_stage_draw* stage_drawing,
        sarena* arena)
{
    if(!renderer) return false;

    struct ntg_event_renderer_rndr_dt event_dt = {
        .drawing = stage_drawing,
        .arena = arena
    };
    ntg_entity_event_raise(ntg_ent(renderer), NTG_EVENT_RENDERER_RNDRPRE, &event_dt);

    struct ntg_renderer_vtable* vtable = ntg_rnd_vtbl(renderer);

    bool rval = false;
    if(vtable->render_fn)
        rval = vtable->render_fn(renderer, stage_drawing, arena);

    ntg_entity_event_raise(ntg_ent(renderer), NTG_EVENT_RENDERER_RNDRPOST, &event_dt);

    return rval;
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg_renderer_init_inherit(
        ntg_renderer* renderer,
        const struct ntg_renderer_vtable* vtable,
        const ntg_type* type)
{
    if(!renderer || !vtable || !type)
        return NTG_ERR_INV_ARG;

    if(!vtable->render_fn)
        return NTG_ERR_BAD_VTABLE;

    if(!ntg_type_instanceof(type, &NTG_TYPE_RENDERER))
        return NTG_ERR_BAD_TYPE;

    int status = ntg_entity_init_inherit(ntg_ent(renderer), &vtable->base, type);
    NTG_POST_INHERIT_CHECK_VTABLE(status);

    ntg_entity_zero(renderer);

    return 0;
}

int ntg_renderer_deinit(ntg_renderer* renderer)
{
    if(!renderer) return NTG_ERR_INV_ARG;

    ntg_entity_zero(renderer);
    ntg_entity_deinit(ntg_ent(renderer));

    return 0;
}
