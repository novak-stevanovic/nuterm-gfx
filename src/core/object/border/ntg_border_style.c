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
        ntg_object_tmp_draw* out_drawing);

static void border_style_deinit_fn(ntg_border_style* style);

static const struct ntg_border_style_vtable VTABLE_DEFAULT = {
    .draw_fn = border_style_draw_fn,
    .deinit_fn = border_style_deinit_fn
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
    .priv.vtable = &VTABLE_DEFAULT,
    .pub.data = NULL
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

void ntg_border_style_vdeinit(ntg_border_style* style)
{
    if(!style) return;
    if(!style->priv.vtable) return;

    if(style->priv.vtable->deinit_fn)
        style->priv.vtable->deinit_fn(style);

}

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
        const struct ntg_border_style_vtable* vtable)
{
    if(!style)
        return NTG_ERR_INV_ARG;

    if(!vtable || !vtable->deinit_fn)
        return NTG_ERR_BAD_VTABLE;

    (*style) = (ntg_border_style) {
        .priv.vtable = vtable,
        .pub.data = NULL
    };
    return 0;
}

int ntg_border_style_deinit(ntg_border_style* style)
{
    if(!style) return NTG_ERR_INV_ARG;

    (*style) = (ntg_border_style) {0};

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
        ntg_object_tmp_draw* out_drawing)
{
    if(!style || !out_drawing) return;
    if(!style->priv.vtable || !style->priv.vtable->draw_fn) return;

    style->priv.vtable->draw_fn(style, size, border_size, out_drawing);

}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static void border_style_deinit_fn(ntg_border_style* style)
{
    ntg_border_style_deinit(style);
}

static void border_style_draw_fn(
        const ntg_border_style* style,
        struct ntg_xy size,
        struct ntg_insets border_size,
        ntg_object_tmp_draw* out_drawing)
{
    (void)style;

    if(ntg_xy_is_zero_any(size)) return;
    if(ntg_insets_is_zero(border_size)) return;

    size_t i, j;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            ntg_object_tmp_draw_set(
                    out_drawing,
                    ntg_vcell_new_default(),
                    ntg_xy(j, i));
        }
    }
}
