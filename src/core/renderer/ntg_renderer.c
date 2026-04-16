#include "ntg.h"
#include <assert.h>
#include "shared/ntg_shared_internal.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_renderer_render(
        ntg_renderer* renderer,
        const ntg_stage_drawing* stage_drawing,
        sarena* arena)
{
    assert(renderer != NULL);

    renderer->__vtable.render_fn(renderer, stage_drawing, arena);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_renderer_init(
        ntg_renderer* renderer,
        const struct ntg_renderer_vtable* vtable)
{
    assert(renderer != NULL);
    assert(vtable != NULL);

    (*renderer) = (ntg_renderer) {0};

    renderer->__vtable = *vtable;
    renderer->data = NULL;
}

void ntg_renderer_deinit(ntg_renderer* renderer)
{
    renderer->__vtable = (struct ntg_renderer_vtable) {0};
    renderer->data = NULL;
}
