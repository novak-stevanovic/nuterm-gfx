#ifndef NTG_COLOR_BLOCK_H
#define NTG_COLOR_BLOCK_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_object.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

struct ntg_color_block_hooks
{
    void (*on_color_chng_fn)(
            ntg_color_block* cb,
            struct nt_color old_color,
            struct nt_color new_color);
};

struct ntg_color_block
{
    ntg_object __base;
    struct nt_color _color;

    struct ntg_color_block_hooks hooks;
};

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* Initializes a drawable object that fills its area with `color`. The current implementation */
/* ignores the status returned by base-object initialization and continues configuring the */
/* object. */

/* ERROR CODES: */
/* - `NTG_ERR_INVALID_ARG`: `color_block` is `NULL`. */
void ntg_color_block_init(
        ntg_color_block* color_block,
        struct nt_color color,
        int* out_status);
/* Releases resources owned by a color block. Passing `NULL` has no effect. */
void ntg_color_block_deinit(ntg_color_block* block);
/* Void-pointer adapter for `ntg_color_block_deinit`, intended for cleanup callbacks. */
void ntg_color_block_deinit_(void* _block);

/* Changes the fill color, invokes the color-change hook, and marks the object for redraw when */
/* the value changes. A `NULL` object is ignored. */
void ntg_color_block_set_color(ntg_color_block* color_block, struct nt_color color);

#endif // NTG_COLOR_BLOCK_H
