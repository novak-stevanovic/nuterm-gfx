#ifndef _NTG_RENDERER_H_
#define _NTG_RENDERER_H_

#include "shared/ntg_xy.h"

/* -------------------------------------------------------------------------- */
/* DECLARATIONS */
/* -------------------------------------------------------------------------- */

typedef struct ntg_renderer ntg_renderer;
typedef struct ntg_stage_drawing ntg_stage_drawing;

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

/* If `stage_drawing` is NULL, render empty */
typedef void (*ntg_renderer_render_fn)(
        ntg_renderer* renderer,
        const ntg_stage_drawing* stage_drawing,
        struct ntg_xy size);

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void _ntg_renderer_init_(
        ntg_renderer* renderer,
        ntg_renderer_render_fn render_fn);
void _ntg_renderer_deinit_(ntg_renderer* renderer);

/* If `stage_drawing` is NULL, render empty */
void ntg_renderer_render(
        ntg_renderer* renderer,
        const ntg_stage_drawing* stage_drawing,
        struct ntg_xy size);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

struct ntg_renderer
{
    ntg_renderer_render_fn __render_fn;
    void* data;
};

#endif // _NTG_RENDERER_H_
