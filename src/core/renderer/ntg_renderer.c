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

int ntg_renderer_vdeinit(ntg_renderer* renderer)
{
    if(!renderer)
        return NTG_ERR_INV_ARG;

    if(renderer->__vtable->deinit_fn)
        renderer->__vtable->deinit_fn(renderer);

    return 0;
}

int ntg_renderer_render(
        ntg_renderer* renderer,
        const ntg_stage_drawing* stage_drawing,
        sarena* arena)
{
    if(!renderer)
        return NTG_ERR_INV_ARG;

    if(renderer->__vtable && renderer->__vtable->render_fn)
    {
        int status = renderer->__vtable->render_fn(renderer, stage_drawing, arena);
        if(status != 0)
            return status;
    }

    struct ntg_event_renderer_onrndr_dt event_dt = {
        .drawing = stage_drawing,
        .arena = arena
    };
    ntg_event_raise(
            &renderer->event_delegate,
            ntg_event_new(NTG_EVENT_RENDERER_ONRNDR, renderer, &event_dt));

    return 0;
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
        const struct ntg_renderer_vtable* vtable)
{
    if(!renderer)
        return NTG_ERR_INV_ARG;

    if(!vtable || !vtable->render_fn || !vtable->deinit_fn)
        return NTG_ERR_BAD_VTABLE;

    (*renderer) = (ntg_renderer) {0};

    renderer->__vtable = vtable;

    ntg_event_delegate_init(&renderer->event_delegate);

    return 0;
}

int ntg_renderer_deinit(ntg_renderer* renderer)
{
    if(!renderer) return NTG_ERR_INV_ARG;

    ntg_event_delegate_deinit(&renderer->event_delegate);

    (*renderer) = (ntg_renderer) {0};

    return 0;
}
