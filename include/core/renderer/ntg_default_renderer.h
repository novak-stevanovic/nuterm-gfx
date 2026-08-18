#ifndef NTG_DEFAULT_RENDERER_H
#define NTG_DEFAULT_RENDERER_H

#include "shared/ntg_shared.h"
#include "core/renderer/ntg_renderer.h"
#include "core/stage/ntg_stage_drawing.h"

#define NTG_DEFAULT_RENDERER_TERM_SIZE_AUTO 50000

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_default_renderer
{
    ntg_renderer __base;

    ntg_stage_drawing __backbuff;

    struct ntg_xy __old_size;
    bool __force_full_render;

    char* __term_buff;
    size_t __term_buff_size;
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

NTG_API void
ntg_default_renderer_init(
        ntg_default_renderer* renderer,
        size_t term_buff_size,
        int* out_status);

NTG_API void
ntg_default_renderer_deinit(ntg_default_renderer* renderer);

NTG_API void
ntg_default_renderer_deinit_void(void* _renderer);

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

NTG_API void
ntg_default_renderer_render_fn(
        ntg_renderer* _renderer,
        const ntg_stage_drawing* stage_drawing,
        sarena* arena,
        int* out_status);

NTG_API void
ntg_default_renderer_deinit_fn(ntg_renderer* _renderer);

NTG_API extern const struct ntg_renderer_vtable NTG_DEFAULT_RENDERER_VTABLE;

#endif // NTG_DEFAULT_RENDERER_H
