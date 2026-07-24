#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

static const struct ntg_renderer_vtable VTABLE_EMPTY = {0};

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

void ntg_renderer_init(ntg_renderer* renderer, int* out_status)
{
    ntg_renderer_init_override(
            renderer,
            &NTG_RENDERER_VTABLE_DEFAULT,
            out_status);
}

void ntg_renderer_deinit(ntg_renderer* renderer)
{
    if(!renderer) return;

    (*renderer) = (ntg_renderer) {0};
}

/* ------------------------------------------------------ */
/* RENDER */
/* ------------------------------------------------------ */

void ntg_renderer_render(
        ntg_renderer* renderer,
        const ntg_stage_drawing* stage_drawing,
        sarena* arena)
{
    if(!renderer) return;

    if(renderer->__vtable && renderer->__vtable->render_fn)
        renderer->__vtable->render_fn(renderer, stage_drawing, arena);

    if(renderer->hooks.on_render_fn)
        renderer->hooks.on_render_fn(renderer, stage_drawing, arena);
}

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void ntg_renderer_init_override(
        ntg_renderer* renderer,
        const struct ntg_renderer_vtable* vtable,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!renderer)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    (*renderer) = (ntg_renderer) {0};
    renderer->__vtable = (vtable ? vtable : &VTABLE_EMPTY);
}

void ntg_renderer_render_fn(
        ntg_renderer* renderer,
        const ntg_stage_drawing* stage_drawing,
        sarena* arena)
{
    (void)renderer;
    (void)stage_drawing;
    (void)arena;
}

const struct ntg_renderer_vtable NTG_RENDERER_VTABLE_DEFAULT = {
    .render_fn = ntg_renderer_render_fn
};
