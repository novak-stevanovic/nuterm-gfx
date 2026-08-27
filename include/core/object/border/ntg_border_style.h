#ifndef NTG_BORDER_STYLE_H
#define NTG_BORDER_STYLE_H

#include "shared/ntg_shared.h"
#include "base/ntg_xy.h"
#include "nt_gfx.h"
#include "base/entity/ntg_entity.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

struct ntg_border_style
{
    ntg_entity _base;

    struct
    {
        void* data;
    } pub;
};

NTG_API extern const ntg_border_style
NTG_BORDER_STYLE_DEFAULT;

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

struct ntg_border_style_vtable
{
    struct ntg_entity_vtable base;

    void (*draw_fn)(
            const ntg_border_style* style,
            struct ntg_xy size,
            struct ntg_insets border_size,
            ntg_object_tmp_draw* out_drawing);

};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_border_style_init_inherit(
        ntg_border_style* style,
        const struct ntg_border_style_vtable* vtable,
        const ntg_type* type);

NTG_API int
ntg_border_style_deinit(ntg_border_style* style);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

void ntg__border_style_draw(
        const ntg_border_style* style,
        struct ntg_xy size,
        struct ntg_insets border_size,
        ntg_object_tmp_draw* out_drawing);

#endif // NTG_BORDER_STYLE_H
