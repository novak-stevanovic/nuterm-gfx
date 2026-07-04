#ifndef NTG_DEF_RENDERER_H
#define NTG_DEF_RENDERER_H

#include "shared/ntg_shared.h"
#include "core/renderer/ntg_renderer.h"
#include "core/stage/ntg_stage_drawing.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

struct ntg_def_renderer
{
    ntg_renderer __base;

    ntg_stage_drawing __backbuff;

    struct ntg_xy __old_size;
};

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* INIT/DEINIT */
/* -------------------------------------------------------------------------- */

/* Initializes the default terminal renderer and its empty back buffer.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `renderer` is `NULL`.
 * - `NTG_ERR_UNEXPECTED`: back-buffer initialization fails unexpectedly. */
NTG_API void
ntg_def_renderer_init(ntg_def_renderer* renderer, int* out_status);

/* -------------------------------------------------------------------------- */

/* Releases resources owned by the default renderer. Passing `NULL` has no
 * effect. */
NTG_API void
ntg_def_renderer_deinit(ntg_def_renderer* renderer);

/* -------------------------------------------------------------------------- */

/* Void-pointer adapter for `ntg_def_renderer_deinit`, intended for cleanup
 * callbacks. */
NTG_API void
ntg_def_renderer_deinit_(void* _renderer);

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* Default renderer callback that converts the stage drawing into terminal
 * output using the supplied arena. */
void _ntg_def_renderer_render_fn(
        ntg_renderer* _renderer,
        const ntg_stage_drawing* stage_drawing,
        sarena* arena);

#endif // NTG_DEF_RENDERER_H
