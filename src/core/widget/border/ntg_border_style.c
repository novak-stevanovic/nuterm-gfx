#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static void border_style_draw_fn(
        const ntg_border_style* style,
        struct ntg_xy size,
        struct ntg_insets border_size,
        ntg_widget_tmp_draw* out_drawing);

static void border_style_deinit_fn(ntg_object* _style);

static const struct ntg_border_style_vtable VTABLE_DEFAULT = {
    .base.deinit_fn = border_style_deinit_fn,
    .draw_fn = border_style_draw_fn
};

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

const ntg_border_style NTG_BORDER_STYLE_DEFAULT = {
    ._base.ro.vtable = &VTABLE_DEFAULT.base,
    ._base.ro.type = &NTG_TYPE_BORDER_STYLE,
    .pub.data = NULL
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg_border_style_init_inherit(
        ntg_border_style* style,
        const struct ntg_border_style_vtable* vtable,
        const ntg_type* type)
{
    if(!style || !vtable || !type)
        return NTG_ERR_INV_ARG;

    if(!ntg_type_instanceof(type, &NTG_TYPE_BORDER_STYLE))
        return NTG_ERR_BAD_TYPE;

    int status = ntg_object_init_inherit(ntg_obj(style), &vtable->base, type);
    NTG_POST_INHERIT_CHECK_VTABLE(status);

    ntg_object_zero(style);

    return 0;
}

int ntg_border_style_deinit(ntg_border_style* style)
{
    if(!style) return NTG_ERR_INV_ARG;

    ntg_object_zero(style);

    ntg_object_deinit(ntg_obj(style));

    return 0;
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

void ntg__border_style_draw(
        const ntg_border_style* style,
        struct ntg_xy size,
        struct ntg_insets border_size,
        ntg_widget_tmp_draw* out_drawing)
{
    if(!style || !out_drawing) return;
    if(!ntg_bs_vtbl(style) || !ntg_bs_vtbl(style)->draw_fn) return;

    ntg_bs_vtbl(style)->draw_fn(style, size, border_size, out_drawing);
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static void border_style_deinit_fn(ntg_object* _style)
{
    ntg_border_style_deinit(ntg_bs(_style));
}

static void border_style_draw_fn(
        const ntg_border_style* style,
        struct ntg_xy size,
        struct ntg_insets border_size,
        ntg_widget_tmp_draw* out_drawing)
{
    (void)style;

    if(ntg_xy_is_zero_any(size)) return;
    if(ntg_insets_is_zero(border_size)) return;

    size_t i, j;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
            ntg_widget_tmp_draw_set(out_drawing, NTG_VCELL_ZERO, ntg_xy_new(j, i));
        
    }
}
