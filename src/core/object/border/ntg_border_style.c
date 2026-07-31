#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

static void border_style_draw_fn(
        const ntg_border_style* style,
        struct ntg_xy size,
        struct ntg_insets border_size,
        ntg_object_tmp_drawing* out_drawing);

static const struct ntg_border_style_vtable VTABLE_DEFAULT = {
    .draw_fn = border_style_draw_fn,
    .deinit_fn = ntg_border_style_deinit
};

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

const ntg_border_style NTG_BORDER_STYLE_DEFAULT = {
    .__vtable = &VTABLE_DEFAULT,
    .data = NULL
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void ntg_border_style_vdeinit(ntg_border_style* style)
{
    if(!style || !style->__vtable) return;

    if(style->__vtable->deinit_fn)
        style->__vtable->deinit_fn(style);
}

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void ntg_border_style_init_inherit(
        ntg_border_style* style,
        const struct ntg_border_style_vtable* vtable,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!style)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    if(!vtable || !vtable->deinit_fn)
        ntg_vreturn(out_status, NTG_ERR_BAD_VTABLE);

    (*style) = (ntg_border_style) {
        .__vtable = vtable,
        .data = NULL
    };
}

void ntg_border_style_deinit(ntg_border_style* style)
{
    if(!style) return;

    (*style) = (ntg_border_style) {0};
}

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void _ntg_border_style_draw(
        const ntg_border_style* style,
        struct ntg_xy size,
        struct ntg_insets border_size,
        ntg_object_tmp_drawing* out_drawing)
{
    if(!style || !style->__vtable || !style->__vtable->draw_fn)
        return;

    style->__vtable->draw_fn(style, size, border_size, out_drawing);
}

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

static void border_style_draw_fn(
        const ntg_border_style* style,
        struct ntg_xy size,
        struct ntg_insets border_size,
        ntg_object_tmp_drawing* out_drawing)
{
    if(ntg_xy_size_is_zero(size)) return;
    if(ntg_insets_is_zero(border_size)) return;

    size_t i, j;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            ntg_object_tmp_drawing_set(
                    out_drawing,
                    ntg_vcell_def(),
                    ntg_xy(j, i));
        }
    }
}
