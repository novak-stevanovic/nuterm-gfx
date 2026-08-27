#ifndef NTG_CLR_BLOCK_H
#define NTG_CLR_BLOCK_H

#include "shared/ntg_shared.h"
#include "core/widget/ntg_widget.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_clr_block
{
    ntg_widget _base;

    struct
    {
        nt_color color;
    } ro;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

NTG_API int
ntg_clr_block_init(ntg_clr_block* clr_block, nt_color color);

NTG_API int
ntg_clr_block_deinit(ntg_clr_block* clr_block);

/* ------------------------------------------------------ */
/* COLOR */
/* ------------------------------------------------------ */

NTG_API int
ntg_clr_block_set_color(ntg_clr_block* clr_block, nt_color color);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_clr_block_vtable
{
    struct ntg_widget_vtable base;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_clr_block_init_inherit(
        ntg_clr_block* clr_block,
        const struct ntg_clr_block_vtable* vtable,
        const ntg_type* type,
        struct ntg_widget_layout_dt* layout_dt);

/* ------------------------------------------------------ */
/* IMPLEMENT */
/* ------------------------------------------------------ */

NTG_API extern const struct ntg_clr_block_vtable NTG_CLR_BLOCK_VTABLE;

NTG_API int
ntg_clr_block_measure_fn(
        const ntg_widget* _clr_block,
        struct ntg_widget_layout_dt* layout_dt,
        enum ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        struct ntg_widget_measure* out_measure);

/* ------------------------------------------------------ */

NTG_API int
ntg_clr_block_draw_fn(
        const ntg_widget* _clr_block,
        struct ntg_widget_layout_dt* layout_dt,
        ntg_widget_tmp_draw* out_drawing,
        sarena* arena);

/* ------------------------------------------------------ */

NTG_API void
ntg_clr_block_deinit_fn(ntg_object* _clr_block);

#endif // NTG_CLR_BLOCK_H
