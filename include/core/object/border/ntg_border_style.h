#ifndef NTG_BORDER_STYLE_H
#define NTG_BORDER_STYLE_H

#include "shared/ntg_shared.h"
#include "base/ntg_xy.h"
#include "nt_gfx.h"

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

struct ntg_border_style
{
    const struct ntg_border_style_vtable* __vtable;
    void* data;
};

NTG_API extern const struct ntg_border_style NTG_BORDER_STYLE_DEFAULT;

void ntg_border_style_vdeinit(ntg_border_style* style);

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

struct ntg_border_style_vtable
{
    void (*draw_fn)(
            const ntg_border_style* style,
            struct ntg_xy size,
            struct ntg_insets border_size,
            ntg_object_tmp_drawing* out_drawing);

    void (*deinit_fn)(ntg_border_style* style);
};

NTG_API int
ntg_border_style_init_inherit(
        ntg_border_style* style,
        const struct ntg_border_style_vtable* vtable);

NTG_API void
ntg_border_style_deinit(ntg_border_style* style);

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

void _ntg_border_style_draw(
        const ntg_border_style* style,
        struct ntg_xy size,
        struct ntg_insets border_size,
        ntg_object_tmp_drawing* out_drawing);

#endif // NTG_BORDER_STYLE_H
