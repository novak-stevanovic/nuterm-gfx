#ifndef NTG_COLOR_BLOCK_H
#define NTG_COLOR_BLOCK_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_object.h"

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_color_block_hooks
{
    void (*on_color_chng_fn)(
            ntg_color_block* color_block,
            struct nt_color old_color,
            struct nt_color new_color);
};

/* ------------------------------------------------------ */

struct ntg_color_block
{
    ntg_object __base;

    struct nt_color _color;
    struct ntg_color_block_hooks hooks;
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */


NTG_API void
ntg_color_block_init(
        ntg_color_block* color_block,
        struct nt_color color,
        int* out_status);

/* ------------------------------------------------------ */


NTG_API void
ntg_color_block_deinit(ntg_color_block* color_block);

/* ------------------------------------------------------ */


NTG_API void
ntg_color_block_deinit_void(void* _color_block);

/* ------------------------------------------------------ */
/* COLOR */
/* ------------------------------------------------------ */


NTG_API void
ntg_color_block_set_color(
        ntg_color_block* color_block,
        struct nt_color color);

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */


NTG_API void
ntg_color_block_init_inherit(
        ntg_color_block* color_block,
        const struct ntg_object_vtable* vtable,
        const ntg_type* type,
        struct ntg_object_layout_dt* layout_dt,
        int* out_status);

/* ------------------------------------------------------ */


NTG_API struct ntg_object_measure
ntg_color_block_measure_fn(
        const ntg_object* _color_block,
        struct ntg_object_layout_dt* layout_dt,
        enum ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        int* out_status);

/* ------------------------------------------------------ */


NTG_API void
ntg_color_block_draw_fn(
        const ntg_object* _color_block,
        struct ntg_object_layout_dt* layout_dt,
        ntg_object_tmp_drawing* out_drawing,
        sarena* arena,
        uint32_t* relayout,
        int* out_status);

/* ------------------------------------------------------ */


NTG_API void
ntg_color_block_deinit_fn(ntg_object* _color_block);


NTG_API extern const struct ntg_object_vtable NTG_COLOR_BLOCK_VTABLE;

#endif // NTG_COLOR_BLOCK_H
