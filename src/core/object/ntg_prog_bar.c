#include "ntg.h"
#include <math.h>
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_prog_bar_style ntg_prog_bar_style_default(void)
{
    return (struct ntg_prog_bar_style) {
        .complete = ntg_vcell_new_full_bg(nt_color_new_auto(0, 255, 0)),
        .uncomplete = ntg_vcell_new_full_bg(nt_color_new_auto(255, 0, 0)),
        .threshold = ntg_vcell_new_full_bg(nt_color_new_auto(0, 255, 0))
    };
}

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

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

void ntg_prog_bar_init(
        ntg_prog_bar* prog_bar,
        const struct ntg_prog_bar_opts* opts,
        int* out_status)
{
    int _status;

    ntg_prog_bar_init_inherit(
            prog_bar,
            &NTG_PROG_BAR_VTABLE,
            &NTG_TYPE_PROG_BAR,
            NULL,
            &_status);

    if(!_status)
        ntg_prog_bar_set_opts(prog_bar, opts);

    ntg_vreturn(out_status, _status);
}

void ntg_prog_bar_deinit(ntg_prog_bar* prog_bar)
{
    if(!prog_bar) return;

    prog_bar->_prog = 0.0;
    prog_bar->_opts = ntg_prog_bar_opts_default();
    prog_bar->hooks = (struct ntg_prog_bar_hooks) {0};

    ntg_object_deinit((ntg_object*)prog_bar);
}

void ntg_prog_bar_deinit_void(void* _prog_bar)
{
    ntg_prog_bar_deinit(_prog_bar);
}

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

void ntg_prog_bar_set_opts(
        ntg_prog_bar* prog_bar,
        const struct ntg_prog_bar_opts* opts)
{
    if(!prog_bar) return;

    struct ntg_prog_bar_opts old_opts = prog_bar->_opts;
    struct ntg_prog_bar_opts new_opts =
            (opts ? (*opts) : ntg_prog_bar_opts_default());

    if(ntg_prog_bar_opts_are_eql(&old_opts, &new_opts))
        return;

    prog_bar->_opts = new_opts;

    ntg_object_mark_dirty((ntg_object*)prog_bar, NTG_OBJECT_DIRTY_FULL);

    if(prog_bar->hooks.on_opts_chng_fn)
        prog_bar->hooks.on_opts_chng_fn(prog_bar, &old_opts, &new_opts);
}

/* ------------------------------------------------------ */
/* PROGRESS */
/* ------------------------------------------------------ */

void ntg_prog_bar_set_prog(ntg_prog_bar* prog_bar, double progress)
{
    if(!prog_bar) return;

    progress = _max2_double(0.0, _min2_double(1.0, progress));

    double old_progress = prog_bar->_prog;

    if(_double_are_eql(old_progress, progress))
        return;

    prog_bar->_prog = progress;

    ntg_object_mark_dirty((ntg_object*)prog_bar, NTG_OBJECT_DIRTY_DRAW);

    if(prog_bar->hooks.on_prog_chng_fn)
        prog_bar->hooks.on_prog_chng_fn(
                prog_bar,
                old_progress,
                progress);
}

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void ntg_prog_bar_init_inherit(
        ntg_prog_bar* prog_bar,
        const struct ntg_object_vtable* vtable,
        const ntg_type* type,
        struct ntg_object_layout_dt* layout_dt,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!prog_bar || !type)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    if(!ntg_type_instance_of(type, &NTG_TYPE_PROG_BAR))
        ntg_vreturn(out_status, NTG_ERR_INVALID_TYPE);

    int _status;

    ntg_object_init_inherit(
            (ntg_object*)prog_bar, vtable, type, layout_dt, &_status);
    switch(_status)
    {
        case 0:
            break;
        case NTG_ERR_ALLOC_FAIL:
            ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);
        default:
            ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
        
    }

    prog_bar->_prog = 0.0;
    prog_bar->_opts = ntg_prog_bar_opts_default();
    prog_bar->hooks = (struct ntg_prog_bar_hooks) {0};
}

struct ntg_object_measure ntg_prog_bar_measure_fn(
        const ntg_object* _prog_bar,
        struct ntg_object_layout_dt* layout_dt,
        enum ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        int* out_status)
{
    (void)layout_dt;
    (void)arena;
    (void)relayout;
    ntg_init_status(out_status);

    const ntg_prog_bar* prog_bar = (const ntg_prog_bar*)_prog_bar;

    if(orient == prog_bar->_opts.orient)
    {
        return (struct ntg_object_measure) {
            .min_size = 10,
            .nat_size = 10,
            .max_size = NTG_SIZE_MAX,
            .grow = 1
        };
    }

    return (struct ntg_object_measure) {
        .min_size = 1,
        .nat_size = 1,
        .max_size = NTG_SIZE_MAX,
        .grow = 1
    };
}

void ntg_prog_bar_draw_fn(
        const ntg_object* _prog_bar,
        struct ntg_object_layout_dt* layout_dt,
        ntg_object_tmp_drawing* out_drawing,
        sarena* arena,
        uint32_t* relayout,
        int* out_status)
{
    (void)layout_dt;
    (void)arena;
    (void)relayout;
    ntg_init_status(out_status);

    const ntg_prog_bar* prog_bar = (const ntg_prog_bar*)_prog_bar;
    struct ntg_xy size = ntg_object_get_size_cont(_prog_bar);

    if(ntg_xy_size_is_zero(size)) return;

    struct ntg_oxy _size =
            ntg_oxy_from_xy(size, prog_bar->_opts.orient);

    size_t complete_count = round(_size.prim_val * prog_bar->_prog);

    size_t i, j;
    struct ntg_oxy _it_xy;
    struct ntg_xy it_xy;
    struct ntg_vcell it_cell;
    for(i = 0; i < _size.prim_val; i++)
    {
        for(j = 0; j < _size.sec_val; j++)
        {
            _it_xy = ntg_oxy(i, j, prog_bar->_opts.orient);
            it_xy = ntg_xy_from_oxy(_it_xy);

            if(complete_count == _size.prim_val)
                it_cell = prog_bar->_opts.style.complete;
            else if(complete_count == 0)
                it_cell = prog_bar->_opts.style.uncomplete;
            else if(i < (complete_count - 1))
                it_cell = prog_bar->_opts.style.complete;
            else if(i == (complete_count - 1))
                it_cell = prog_bar->_opts.style.threshold;
            else
                it_cell = prog_bar->_opts.style.uncomplete;

            ntg_object_tmp_drawing_set(out_drawing, it_cell, it_xy);
        }
    }
}

void ntg_prog_bar_deinit_fn(ntg_object* _prog_bar)
{
    ntg_prog_bar_deinit((ntg_prog_bar*)_prog_bar);
}

const struct ntg_object_vtable NTG_PROG_BAR_VTABLE = {
    .measure_fn = ntg_prog_bar_measure_fn,
    .draw_fn = ntg_prog_bar_draw_fn,
    .deinit_fn = ntg_prog_bar_deinit_fn
};
