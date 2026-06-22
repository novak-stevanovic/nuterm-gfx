#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_renderer_render(
        ntg_renderer* renderer,
        const ntg_stage_drawing* stage_drawing,
        sarena* arena)
{
    if(!renderer) return;

    renderer->__vtable.render_fn(renderer, stage_drawing, arena);

    if(renderer->hooks.on_render_fn)
        renderer->hooks.on_render_fn(renderer, stage_drawing, arena);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_renderer_init(
        ntg_renderer* renderer,
        const struct ntg_renderer_vtable* vtable,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!renderer || !vtable)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    if(!vtable->render_fn)
        ntg_vreturn(out_status, NTG_ERR_VTABLE_NO_IMPL);

    (*renderer) = (ntg_renderer) {0};

    renderer->__vtable = *vtable;
    renderer->data = NULL;
}

void ntg_renderer_deinit(ntg_renderer* renderer)
{
    if(!renderer) return;

    renderer->__vtable = (struct ntg_renderer_vtable) {0};
    renderer->data = NULL;
}
