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

    if(renderer->priv.vtable->deinit_fn)
        renderer->priv.vtable->deinit_fn(renderer);

    return 0;
}

int ntg_renderer_render(
        ntg_renderer* renderer,
        const ntg_stage_draw* stage_drawing,
        sarena* arena)
{
    if(!renderer)
        return NTG_ERR_INV_ARG;

    struct ntg_event_renderer_rndr_dt event_dt = {
        .drawing = stage_drawing,
        .arena = arena
    };
    ntg_event_raise(
            &renderer->pub.event_delegate,
            ntg_event_new(NTG_EVENT_RENDERER_RNDRPRE, renderer, &event_dt));

    int status = 0;
    if(renderer->priv.vtable && renderer->priv.vtable->render_fn)
        status = renderer->priv.vtable->render_fn(renderer, stage_drawing, arena);

    ntg_event_raise(
            &renderer->pub.event_delegate,
            ntg_event_new(NTG_EVENT_RENDERER_RNDRPOST, renderer, &event_dt));

    return status;
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

    renderer->priv.vtable = vtable;

    ntg_event_delegate_init(&renderer->pub.event_delegate);

    return 0;
}

int ntg_renderer_deinit(ntg_renderer* renderer)
{
    if(!renderer) return NTG_ERR_INV_ARG;

    ntg_event_delegate_deinit(&renderer->pub.event_delegate);

    (*renderer) = (ntg_renderer) {0};

    return 0;
}
