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

    renderer->__render_fn(renderer, stage_drawing, arena);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_renderer_init(ntg_renderer* renderer, ntg_renderer_render_fn render_fn)
{
    assert(renderer != NULL);
    assert(render_fn != NULL);

    renderer->__render_fn = render_fn;
    renderer->data = NULL;
}

void ntg_renderer_deinit(ntg_renderer* renderer)
{
    renderer->__render_fn = NULL;
    renderer->data = NULL;
}
