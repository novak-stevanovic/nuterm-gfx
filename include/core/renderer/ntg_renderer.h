#ifndef NTG_RENDERER_H
#define NTG_RENDERER_H

#include "shared/ntg_shared.h"
#include "base/ntg_event.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_renderer
{
    struct
    {
        ntg_event_delegate event_delegate;
        void* data;
    } pub;

    struct
    {
        const struct ntg_renderer_vtable* vtable;
    } priv;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_renderer_vdeinit(ntg_renderer* renderer);

NTG_API int
ntg_renderer_render(
        ntg_renderer* renderer,
        const ntg_stage_draw* stage_drawing,
        sarena* arena);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_renderer_vtable
{
    int (*render_fn)(
            ntg_renderer* renderer,
            const ntg_stage_draw* stage_drawing,
            sarena* arena);

    void (*loop_enter_fn)(ntg_renderer* renderer);
    void (*loop_leave_fn)(ntg_renderer* renderer);

    void (*deinit_fn)(ntg_renderer* renderer);
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_renderer_init_inherit(
        ntg_renderer* renderer,
        const struct ntg_renderer_vtable* vtable);

NTG_API int
ntg_renderer_deinit(ntg_renderer* renderer);

#endif // NTG_RENDERER_H
