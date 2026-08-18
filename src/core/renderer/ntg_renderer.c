#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

void ntg_renderer_vdeinit(ntg_renderer* renderer)
{
    if(!renderer)
        return;

    if(renderer->__vtable->deinit_fn)
        renderer->__vtable->deinit_fn(renderer);
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

    if(renderer->hooks.on_render_fn)
        renderer->hooks.on_render_fn(renderer, stage_drawing, arena);

    return 0;
}

/* ========================================================================== */
/* PROTECTED */
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
    return 0;
}

void ntg_renderer_deinit(ntg_renderer* renderer)
{
    if(!renderer) return;

    (*renderer) = (ntg_renderer) {0};
}
