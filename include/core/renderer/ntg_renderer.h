#ifndef NTG_RENDERER_H
#define NTG_RENDERER_H

#include "shared/ntg_shared.h"
#include "base/entity/ntg_entity.h"

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
    ntg_entity _base;
    struct
    {
        void* data;
    } pub;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

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
    struct ntg_entity_vtable base;

    int (*render_fn)(
            ntg_renderer* renderer,
            const ntg_stage_draw* stage_drawing,
            sarena* arena);

    void (*loop_enter_fn)(ntg_renderer* renderer);
    void (*loop_leave_fn)(ntg_renderer* renderer);
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_renderer_init_inherit(
        ntg_renderer* renderer,
        const struct ntg_renderer_vtable* vtable,
        const ntg_type* type);

NTG_API int
ntg_renderer_deinit(ntg_renderer* renderer);

#endif // NTG_RENDERER_H
