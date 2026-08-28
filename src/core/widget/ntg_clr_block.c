#include "ntg.h"
#include "shared/ntg_shared_internal.h"

#define DEFAULT_SIZE 1

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

int ntg_clr_block_init(ntg_clr_block* clr_block, nt_color color)
{
    if(!clr_block) return NTG_ERR_INV_ARG;

    int status = ntg_clr_block_init_inherit(
            clr_block,
            &NTG_CLR_BLOCK_VTABLE,
            &NTG_TYPE_CLR_BLOCK,
            NULL);
    NTG_POST_INHERIT_CHECK(status);

    ntg_clr_block_set_color(clr_block, color);

    return 0;
}

/* ------------------------------------------------------ */

int ntg_clr_block_deinit(ntg_clr_block* clr_block)
{
    if(!clr_block) return NTG_ERR_INV_ARG;

    ntg_object_zero(clr_block);

    ntg_widget_deinit((ntg_widget*)clr_block);

    return 0;
}

/* ------------------------------------------------------ */
/* COLOR */
/* ------------------------------------------------------ */

int ntg_clr_block_set_color(ntg_clr_block* clr_block, nt_color color)
{
    if(!clr_block) return NTG_ERR_INV_ARG;

    nt_color old_color = clr_block->ro.color;

    if(nt_color_are_eql(old_color, color))
        return 0;

    clr_block->ro.color = color;

    ntg_widget_mark_dirty((ntg_widget*)clr_block, NTG_WIDGET_DIRTY_DRAW);

    struct ntg_event_clr_block_clrchg_dt event_dt = {
        .old_color = old_color,
        .new_color = color
    };
    ntg_object_event_raise(ntg_obj(clr_block), NTG_EVENT_CLR_BLOCK_CLRCHG, &event_dt);

    return 0;
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg_clr_block_init_inherit(
        ntg_clr_block* clr_block,
        const struct ntg_clr_block_vtable* vtable,
        const ntg_type* type,
        struct ntg_widget_layout_dt* layout_dt)
{
    if(!clr_block || !type || !vtable)
        return NTG_ERR_INV_ARG;

    if(!ntg_type_instanceof(type, &NTG_TYPE_CLR_BLOCK))
        return NTG_ERR_BAD_TYPE;

    int status = ntg_widget_init_inherit(ntg_wgt(clr_block),
            &vtable->base, type, layout_dt);
    NTG_POST_INHERIT_CHECK_VTABLE(status);

    ntg_object_zero(clr_block);

    return 0;
}

/* ------------------------------------------------------ */
/* IMPLEMENT */
/* ------------------------------------------------------ */

const struct ntg_clr_block_vtable NTG_CLR_BLOCK_VTABLE = {
    .base = {
        .measure_fn = ntg_clr_block_measure_fn,
        .draw_fn = ntg_clr_block_draw_fn,
        .base.deinit_fn = ntg_clr_block_deinit_fn
    }
};

int ntg_clr_block_measure_fn(
        const ntg_widget* _clr_block,
        struct ntg_widget_layout_dt* layout_dt,
        enum ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        struct ntg_widget_measure* out_measure)
{
    (void)_clr_block;
    (void)layout_dt;
    (void)orient;
    (void)arena;
    (void)relayout;
    if(!out_measure)
        return NTG_ERR_INV_ARG;

    *out_measure = (struct ntg_widget_measure) {
        .min_size = DEFAULT_SIZE,
        .nat_size = DEFAULT_SIZE,
        .max_size = NTG_SIZE_MAX,
        .grow = 1
    };
    return 0;
}

/* ------------------------------------------------------ */

int ntg_clr_block_draw_fn(
        const ntg_widget* _clr_block,
        struct ntg_widget_layout_dt* layout_dt,
        ntg_widget_tmp_draw* out_drawing,
        sarena* arena)
{
    (void)layout_dt;
    (void)arena;
    const ntg_clr_block* clr_block = ntg_cb(_clr_block);
    struct ntg_xy size = ntg_widget_get_size_cont(_clr_block);

    if(ntg_xy_is_zero_any(size)) return 0;

    size_t i, j;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            ntg_widget_tmp_draw_set(
                    out_drawing,
                    ntg_vcell_new_full_bg(clr_block->ro.color),
                    ntg_xy_new(j, i));
        }
    }

    return 0;
}

/* ------------------------------------------------------ */

void ntg_clr_block_deinit_fn(ntg_object* _clr_block)
{
    ntg_clr_block_deinit((ntg_clr_block*)_clr_block);
}

