#include "ntg.h"
#include <math.h>
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

int ntg_prog_bar_init(ntg_prog_bar* prog_bar, const struct ntg_prog_bar_opts* opts)
{
    if(!prog_bar) return NTG_ERR_INV_ARG;

    int status = ntg_prog_bar_init_inherit(
            prog_bar,
            &NTG_PROG_BAR_VTABLE,
            &NTG_TYPE_PROG_BAR,
            NULL);
    NTG_POST_INHERIT_CHECK(status);

    ntg_prog_bar_set_opts(prog_bar, opts);

    return 0;
}

/* ------------------------------------------------------ */

int ntg_prog_bar_deinit(ntg_prog_bar* prog_bar)
{
    if(!prog_bar) return NTG_ERR_INV_ARG;

    ntg_object_zero(prog_bar);

    ntg_widget_deinit((ntg_widget*)prog_bar);

    return 0;
}

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

static inline bool
styles_are_eql(struct ntg_prog_bar_style style1, struct ntg_prog_bar_style style2)
{
    return ntg_vcell_are_eql(style1.complete, style2.complete) &&
           ntg_vcell_are_eql(style1.uncomplete, style2.uncomplete) &&
           ntg_vcell_are_eql(style1.threshold, style2.threshold);
}

int ntg_prog_bar_set_opts(ntg_prog_bar* prog_bar, const struct ntg_prog_bar_opts* opts)
{
    if(!prog_bar) return NTG_ERR_INV_ARG;

    /* Set up final opts */

    struct ntg_prog_bar_opts opts_final = (opts ? (*opts) : NTG_PROG_BAR_OPTS_ZERO);
    if(styles_are_eql(opts_final.style, NTG_PROG_BAR_STYLE_ZERO))
    {
        opts_final.style = (struct ntg_prog_bar_style) {
            .complete = ntg_vcell_new_full_bg(nt_color_new_auto(0, 255, 0)),
            .uncomplete = ntg_vcell_new_full_bg(nt_color_new_auto(255, 0, 0)),
            .threshold = ntg_vcell_new_full_bg(nt_color_new_auto(0, 255, 0))
        };
    }

    /* Check for changes */

    uint8_t dirty = 0;

    if(prog_bar->ro.opts.orient != opts_final.orient)
    {
        dirty |= NTG_WIDGET_DIRTY_FULL;
    }
    if(!styles_are_eql(prog_bar->ro.opts.style, opts_final.style))
    {
        dirty |= NTG_WIDGET_DIRTY_DRAW;
    }
    
    if(!dirty) return 0;

    prog_bar->ro.opts.orient = opts_final.orient;
    prog_bar->ro.opts.style = opts_final.style;

    ntg_widget_mark_dirty((ntg_widget*)prog_bar, dirty);

    return 0;
}

/* ------------------------------------------------------ */
/* PROGRESS */
/* ------------------------------------------------------ */

int ntg_prog_bar_set_prog(ntg_prog_bar* prog_bar, double progress)
{
    if(!prog_bar) return NTG_ERR_INV_ARG;

    progress = ntg_max2_double(0.0, ntg_min2_double(1.0, progress));

    double old_progress = prog_bar->ro.prog;

    if(ntg_double_are_eql(old_progress, progress))
        return 0;

    prog_bar->ro.prog = progress;

    ntg_widget_mark_dirty((ntg_widget*)prog_bar, NTG_WIDGET_DIRTY_DRAW);

    struct ntg_event_prog_bar_progchg_dt event_dt = {
        .old_prog = old_progress,
        .new_prog = progress
    };
    ntg_object_event_raise(ntg_obj(prog_bar), NTG_EVENT_PROG_BAR_PROGCHG, &event_dt);

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

int ntg_prog_bar_init_inherit(
        ntg_prog_bar* prog_bar,
        const struct ntg_prog_bar_vtable* vtable,
        const ntg_type* type,
        struct ntg_widget_layout_dt* layout_dt)
{
    if(!prog_bar || !type || !vtable)
        return NTG_ERR_INV_ARG;

    if(!ntg_type_instanceof(type, &NTG_TYPE_PROG_BAR))
        return NTG_ERR_BAD_TYPE;

    int status = ntg_widget_init_inherit(ntg_wgt(prog_bar), &vtable->base, type, layout_dt);
    NTG_POST_INHERIT_CHECK_VTABLE(status);

    ntg_object_zero(prog_bar);

    return 0;
}

/* ------------------------------------------------------ */
/* IMPLEMENT */
/* ------------------------------------------------------ */

const struct ntg_prog_bar_vtable NTG_PROG_BAR_VTABLE = {
    .base = {
        .base = {
            .deinit_fn = ntg_prog_bar_deinit_fn
        },
        .measure_fn = ntg_prog_bar_measure_fn,
        .draw_fn = ntg_prog_bar_draw_fn
    }
};

int ntg_prog_bar_measure_fn(
        const ntg_widget* _prog_bar,
        struct ntg_widget_layout_dt* layout_dt,
        enum ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        struct ntg_widget_measure* out_measure)
{
    (void)layout_dt;
    (void)arena;
    (void)relayout;

    if(!_prog_bar || !out_measure)
        return NTG_ERR_INV_ARG;

    const ntg_prog_bar* prog_bar = (const ntg_prog_bar*)_prog_bar;
    size_t size = (orient == prog_bar->ro.opts.orient) ? 10 : 1;
    *out_measure = (struct ntg_widget_measure) {
        .min_size = size,
        .nat_size = size,
        .max_size = NTG_SIZE_MAX,
        .grow = 1
    };
    return 0;
}

/* ------------------------------------------------------ */

int ntg_prog_bar_draw_fn(
        const ntg_widget* _prog_bar,
        struct ntg_widget_layout_dt* layout_dt,
        ntg_widget_tmp_draw* out_drawing,
        sarena* arena)
{
    (void)layout_dt;
    (void)arena;
    const ntg_prog_bar* prog_bar = (const ntg_prog_bar*)_prog_bar;
    ntg_xy size = ntg_widget_get_size_cont(_prog_bar);

    if(ntg_xy_is_zero_any(size)) return 0;

    enum ntg_orient orient = prog_bar->ro.opts.orient;

    size_t size_prim = ntg_xy_get(size, orient);
    size_t size_sec = ntg_xy_get_other(size, orient);

    size_t complete_count = round(size_prim * prog_bar->ro.prog);

    size_t i, j;
    struct ntg_vcell it_cell;
    for(i = 0; i < size_prim; i++)
    { 
        for(j = 0; j < size_sec; j++)
        {
            if(complete_count == size_prim)
                it_cell = prog_bar->ro.opts.style.complete;
            else if(complete_count == 0)
                it_cell = prog_bar->ro.opts.style.uncomplete;
            else if(i < (complete_count - 1))
                it_cell = prog_bar->ro.opts.style.complete;
            else if(i == (complete_count - 1))
                it_cell = prog_bar->ro.opts.style.threshold;
            else
                it_cell = prog_bar->ro.opts.style.uncomplete;

            ntg_widget_tmp_draw_set(out_drawing, it_cell, ntg_xy_new_orient(i, j, orient));
        }
    }

    return 0;
}

/* ------------------------------------------------------ */

void ntg_prog_bar_deinit_fn(ntg_object* _prog_bar)
{
    ntg_prog_bar_deinit((ntg_prog_bar*)_prog_bar);
}

