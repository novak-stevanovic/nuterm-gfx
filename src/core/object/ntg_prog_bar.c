#include "ntg.h"
#include <math.h>
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_prog_bar_style ntg_prog_bar_style_default(void)
{
    return (struct ntg_prog_bar_style) {
        .complete = ntg_vcell_new_full_bg(nt_color_new_auto(0, 255, 0)),
        .uncomplete = ntg_vcell_new_full_bg(nt_color_new_auto(255, 0, 0)),
        .threshold = ntg_vcell_new_full_bg(nt_color_new_auto(0, 255, 0))
    };
}

/* ------------------------------------------------------ */

bool ntg_prog_bar_style_are_eql(
        const struct ntg_prog_bar_style* style1,
        const struct ntg_prog_bar_style* style2)
{
    if(style1 == style2)
        return true;

    if(!style1 || !style2)
        return false;

    return (ntg_vcell_are_eql(style1->complete,
                             style2->complete) &&
            ntg_vcell_are_eql(style1->uncomplete,
                             style2->uncomplete) &&
            ntg_vcell_are_eql(style1->threshold,
                             style2->threshold));
}

/* ------------------------------------------------------ */

struct ntg_prog_bar_opts ntg_prog_bar_opts_default(void)
{
    return (struct ntg_prog_bar_opts) {
        .orient = NTG_ORIENT_H,
        .style = ntg_prog_bar_style_default()
    };
}

/* ------------------------------------------------------ */

bool ntg_prog_bar_opts_are_eql(
        const struct ntg_prog_bar_opts* opts1,
        const struct ntg_prog_bar_opts* opts2)
{
    if(opts1 == opts2)
        return true;

    if(!opts1 || !opts2)
        return false;

    return (ntg_prog_bar_style_are_eql(&opts1->style, &opts2->style) &&
            (opts1->orient == opts2->orient));
}

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

int ntg_prog_bar_init(
        ntg_prog_bar* prog_bar,
        const struct ntg_prog_bar_opts* opts)
{
    int _status = ntg_object_init_inherit(
            (ntg_object*)prog_bar,
            &NTG_PROG_BAR_OBJECT_IMPL,
            &NTG_TYPE_PROG_BAR,
            NULL);
    if(_status != 0)
        return _status;

    prog_bar->ro.prog = 0.0;
    prog_bar->ro.opts = ntg_prog_bar_opts_default();
    ntg_prog_bar_set_opts(prog_bar, opts);
    return 0;
}

/* ------------------------------------------------------ */

int ntg_prog_bar_deinit(ntg_prog_bar* prog_bar)
{
    if(!prog_bar) return NTG_ERR_INV_ARG;

    prog_bar->ro.prog = 0.0;
    prog_bar->ro.opts = ntg_prog_bar_opts_default();

    ntg_object_deinit((ntg_object*)prog_bar);

    return 0;
}

/* ------------------------------------------------------ */

void ntg_prog_bar_deinit_void(void* _prog_bar)
{
    ntg_prog_bar_deinit(_prog_bar);
}

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

int ntg_prog_bar_set_opts(
        ntg_prog_bar* prog_bar,
        const struct ntg_prog_bar_opts* opts)
{
    if(!prog_bar) return NTG_ERR_INV_ARG;

    struct ntg_prog_bar_opts old_opts = prog_bar->ro.opts;
    struct ntg_prog_bar_opts new_opts =
            (opts ? (*opts) : ntg_prog_bar_opts_default());

    if(ntg_prog_bar_opts_are_eql(&old_opts, &new_opts))
        return 0;

    prog_bar->ro.opts = new_opts;

    ntg_object_mark_dirty((ntg_object*)prog_bar, NTG_OBJECT_DIRTY_FULL);

    struct ntg_event_prog_bar_optchg_dt event_dt = {
        .old_opts = &old_opts,
        .new_opts = &new_opts
    };
    ntg_entity_event_raise(ntg_ent(prog_bar), NTG_EVENT_PROG_BAR_OPTCHG, &event_dt);

    return 0;
}

/* ------------------------------------------------------ */
/* PROGRESS */
/* ------------------------------------------------------ */

int ntg_prog_bar_set_prog(ntg_prog_bar* prog_bar, double progress)
{
    if(!prog_bar) return NTG_ERR_INV_ARG;

    progress = _max2_double(0.0, _min2_double(1.0, progress));

    double old_progress = prog_bar->ro.prog;

    if(_double_are_eql(old_progress, progress))
        return 0;

    prog_bar->ro.prog = progress;

    ntg_object_mark_dirty((ntg_object*)prog_bar, NTG_OBJECT_DIRTY_DRAW);

    struct ntg_event_prog_bar_progchg_dt event_dt = {
        .old_prog = old_progress,
        .new_prog = progress
    };
    ntg_entity_event_raise(ntg_ent(prog_bar), NTG_EVENT_PROG_BAR_PROGCHG, &event_dt);

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
        struct ntg_object_layout_dt* layout_dt)
{
    if(!prog_bar || !type)
        return NTG_ERR_INV_ARG;

    if(!vtable)
        return NTG_ERR_BAD_VTABLE;

    if(!ntg_type_instanceof(type, &NTG_TYPE_PROG_BAR))
        return NTG_ERR_BAD_TYPE;

    int _status = ntg_object_init_inherit(
            (ntg_object*)prog_bar, &vtable->object, type, layout_dt);
    if(_status != 0)
        return _status;

    prog_bar->ro.prog = 0.0;
    prog_bar->ro.opts = ntg_prog_bar_opts_default();
    return 0;
}

/* ------------------------------------------------------ */

int ntg_prog_bar_measure_fn(
        const ntg_object* _prog_bar,
        struct ntg_object_layout_dt* layout_dt,
        enum ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        struct ntg_object_measure* out_measure)
{
    (void)layout_dt;
    (void)arena;
    (void)relayout;

    if(!_prog_bar || !out_measure)
        return NTG_ERR_INV_ARG;

    const ntg_prog_bar* prog_bar = (const ntg_prog_bar*)_prog_bar;
    size_t size = (orient == prog_bar->ro.opts.orient) ? 10 : 1;
    *out_measure = (struct ntg_object_measure) {
        .min_size = size,
        .nat_size = size,
        .max_size = NTG_SIZE_MAX,
        .grow = 1
    };
    return 0;
}

/* ------------------------------------------------------ */

int ntg_prog_bar_draw_fn(
        const ntg_object* _prog_bar,
        struct ntg_object_layout_dt* layout_dt,
        ntg_object_tmp_draw* out_drawing,
        sarena* arena)
{
    (void)layout_dt;
    (void)arena;
    const ntg_prog_bar* prog_bar = (const ntg_prog_bar*)_prog_bar;
    struct ntg_xy size = ntg_object_get_size_cont(_prog_bar);

    if(ntg_xy_is_zero_any(size)) return 0;

    struct ntg_oxy _size =
            ntg_oxy_from_xy(size, prog_bar->ro.opts.orient);

    size_t complete_count = round(_size.prim_val * prog_bar->ro.prog);

    size_t i, j;
    struct ntg_oxy _it_xy;
    struct ntg_xy it_xy;
    struct ntg_vcell it_cell;
    for(i = 0; i < _size.prim_val; i++)
    {
        for(j = 0; j < _size.sec_val; j++)
        {
            _it_xy = ntg_oxy(i, j, prog_bar->ro.opts.orient);
            it_xy = ntg_xy_from_oxy(_it_xy);

            if(complete_count == _size.prim_val)
                it_cell = prog_bar->ro.opts.style.complete;
            else if(complete_count == 0)
                it_cell = prog_bar->ro.opts.style.uncomplete;
            else if(i < (complete_count - 1))
                it_cell = prog_bar->ro.opts.style.complete;
            else if(i == (complete_count - 1))
                it_cell = prog_bar->ro.opts.style.threshold;
            else
                it_cell = prog_bar->ro.opts.style.uncomplete;

            ntg_object_tmp_draw_set(out_drawing, it_cell, it_xy);
        }
    }

    return 0;
}

/* ------------------------------------------------------ */

void ntg_prog_bar_deinit_fn(ntg_entity* _prog_bar)
{
    ntg_prog_bar_deinit((ntg_prog_bar*)_prog_bar);
}

const struct ntg_object_vtable NTG_PROG_BAR_OBJECT_IMPL = {
    .measure_fn = ntg_prog_bar_measure_fn,
    .draw_fn = ntg_prog_bar_draw_fn,
    .base.deinit_fn = ntg_prog_bar_deinit_fn
};
