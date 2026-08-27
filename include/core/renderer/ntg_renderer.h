#ifndef NTG_RENDERER_H
#define NTG_RENDERER_H

#include "shared/ntg_shared.h"
#include "base/object/ntg_object.h"
#include "base/ntg_cell.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

static inline struct ntg_cell
ntg_renderer_cell_normalize(struct ntg_cell cell)
{
    return (struct ntg_cell) {
        .cp = (ntg_cell_cp_is_ws(cell.cp) ? ' ' : cell.cp),
        .gfx = cell.gfx
    };
}

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_renderer
{
    ntg_object _base;
    struct
    {
        void* data;
    } pub;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API bool
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
    struct ntg_object_vtable base;

    bool (*render_fn)(
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
