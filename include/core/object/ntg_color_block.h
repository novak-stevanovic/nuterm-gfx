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

/* Initializes a drawable object that fills its content area with `color`.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `color_block` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: base-object resources cannot be allocated.
 * - `NTG_ERR_UNEXPECTED`: base-object initialization fails unexpectedly. */
NTG_API void
ntg_color_block_init(
        ntg_color_block* color_block,
        struct nt_color color,
        int* out_status);

/* ------------------------------------------------------ */

/* Releases resources owned by a color block. Passing `NULL` has no effect. */
NTG_API void
ntg_color_block_deinit(ntg_color_block* color_block);

/* ------------------------------------------------------ */

/* Void-pointer adapter for `ntg_color_block_deinit`, intended for cleanup
 * callbacks. */
NTG_API void
ntg_color_block_deinit_v(void* _color_block);

/* ------------------------------------------------------ */
/* COLOR */
/* ------------------------------------------------------ */

/* Changes the fill color, invokes the color-change hook, and marks the object
 * for redraw when the value changes. A `NULL` object is ignored. */
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

/* Initializes the color-block portion of an object derived from
 * `NTG_TYPE_COLOR_BLOCK`, using the supplied virtual table and concrete type
 * descriptor.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `color_block` or `type` is `NULL`.
 * - `NTG_ERR_INVALID_TYPE`: `type` is not derived from
 *   `NTG_TYPE_COLOR_BLOCK`.
 * - `NTG_ERR_BAD_VTABLE`: `vtable` or `vtable->deinit_fn` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: base-object resources cannot be allocated.
 * - `NTG_ERR_UNEXPECTED`: base-object initialization fails unexpectedly. */
NTG_API void
ntg_color_block_init_inherit(
        ntg_color_block* color_block,
        const struct ntg_object_vtable* vtable,
        const ntg_type* type,
        int* out_status);

/* ------------------------------------------------------ */

/* Implements color-block measurement for an object virtual table. */
NTG_API struct ntg_object_measure
ntg_color_block_measure_fn(
        const ntg_object* _color_block,
        ntg_orient orient,
        void* _layout_cache,
        sarena* arena);

/* ------------------------------------------------------ */

/* Fills the color-block content drawing with its configured color. */
NTG_API void
ntg_color_block_draw_fn(
        const ntg_object* _color_block,
        ntg_object_tmp_drawing* out_drawing,
        void* _layout_cache,
        sarena* arena);

/* ------------------------------------------------------ */

/* Virtual deinitializer that dispatches to `ntg_color_block_deinit`. */
NTG_API void
ntg_color_block_deinit_fn(ntg_object* _color_block);

/* Default virtual table used by `ntg_color_block_init`. */
NTG_API extern const struct ntg_object_vtable NTG_COLOR_BLOCK_VTABLE;

#endif // NTG_COLOR_BLOCK_H
