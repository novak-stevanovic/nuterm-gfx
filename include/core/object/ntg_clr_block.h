#ifndef NTG_CLR_BLOCK_H
#define NTG_CLR_BLOCK_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_object.h"

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
    struct
    {
        ntg_object base;
    } priv;

    struct
    {
        struct nt_color color;
    } ro;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */


NTG_API int
ntg_clr_block_init(
        ntg_clr_block* clr_block,
        struct nt_color color);

/* ------------------------------------------------------ */


NTG_API int
ntg_clr_block_deinit(ntg_clr_block* clr_block);

/* ------------------------------------------------------ */


NTG_API void
ntg_clr_block_deinit_void(void* _clr_block);

/* ------------------------------------------------------ */
/* COLOR */
/* ------------------------------------------------------ */


NTG_API int
ntg_clr_block_set_color(
        ntg_clr_block* clr_block,
        struct nt_color color);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */


NTG_API int
ntg_clr_block_init_inherit(
        ntg_clr_block* clr_block,
        const struct ntg_object_vtable* vtable,
        const ntg_type* type,
        struct ntg_object_layout_dt* layout_dt);

/* ------------------------------------------------------ */


NTG_API int
ntg_clr_block_measure_fn(
        const ntg_object* _clr_block,
        struct ntg_object_layout_dt* layout_dt,
        enum ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        struct ntg_object_measure* out_measure);

/* ------------------------------------------------------ */


NTG_API int
ntg_clr_block_draw_fn(
        const ntg_object* _clr_block,
        struct ntg_object_layout_dt* layout_dt,
        ntg_object_tmp_drawing* out_drawing,
        sarena* arena);

/* ------------------------------------------------------ */


NTG_API void
ntg_clr_block_deinit_fn(ntg_object* _clr_block);


NTG_API extern const struct ntg_object_vtable NTG_CLR_BLOCK_VTABLE;

#endif // NTG_CLR_BLOCK_H
