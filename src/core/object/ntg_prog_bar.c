#include "ntg.h"
#include <assert.h>
#include <math.h>
#include "shared/ntg_shared_internal.h"

static struct ntg_object_measure measure_fn(
        const ntg_object* _prog_bar,
        ntg_orient orient,
        void* layout_ch,
        sarena* arena);

static void draw_fn(
        const ntg_object* _prog_bar,
        ntg_object_tmp_drawing* out_drawing,
        void* layout_ch,
        sarena* arena);

struct ntg_prog_bar_style ntg_prog_bar_style_def()
{
    return (struct ntg_prog_bar_style) {
        .complete_style = ntg_vcell_bg(nt_color_new_auto(0, 255, 0)),
        .uncomplete_style = ntg_vcell_bg(nt_color_new_auto(255, 0, 0)),
        .threshold_style = ntg_vcell_bg(nt_color_new_auto(0, 255, 0))
    };
}

struct ntg_prog_bar_opts ntg_prog_bar_opts_def()
{
    return (struct ntg_prog_bar_opts) {
        .orient = NTG_ORIENT_H,
        .style = ntg_prog_bar_style_def()
    };
}

bool ntg_prog_bar_opts_are_eq(
        const struct ntg_prog_bar_opts* opts1,
        const struct ntg_prog_bar_opts* opts2)
{
    if(opts1 == opts2)
        return true;

    if(!opts1 || !opts2)
        return false;

    return (ntg_prog_bar_style_are_eq(&opts1->style, &opts2->style) &&
            (opts1->orient == opts2->orient));
}

bool ntg_prog_bar_style_are_eq(
        const struct ntg_prog_bar_style* style1,
        const struct ntg_prog_bar_style* style2)
{
    if(style1 == style2)
        return true;

    if(!style1 || !style2)
        return false;

    return (ntg_vcell_are_equal(style1->complete_style, style2->complete_style) &&
            ntg_vcell_are_equal(style1->uncomplete_style, style2->uncomplete_style) &&
            ntg_vcell_are_equal(style1->threshold_style, style2->threshold_style));
}

void ntg_prog_bar_init(ntg_prog_bar* prog_bar, const struct ntg_prog_bar_opts* opts)
{
    assert(prog_bar != NULL);

    struct ntg_object_vtable vtable = {
        .measure_fn = measure_fn,
        .constrain_fn = NULL,
        .arrange_fn = NULL,
        .draw_fn = draw_fn,
        .rm_child_fn = NULL
    };

    ntg_object_init((ntg_object*)prog_bar, &vtable, &NTG_TYPE_PROG_BAR);

    prog_bar->hooks = (struct ntg_prog_bar_hooks) {0};

    prog_bar->_opts = ntg_prog_bar_opts_def();
    prog_bar->_opts.style = ntg_prog_bar_style_def();

    ntg_prog_bar_set_opts(prog_bar, opts);
}

void ntg_prog_bar_deinit(ntg_prog_bar* prog_bar)
{
    prog_bar->_opts = ntg_prog_bar_opts_def();
    prog_bar->_opts.style = ntg_prog_bar_style_def();

    ntg_object_deinit((ntg_object*)prog_bar);
}

void ntg_prog_bar_deinit_(void* _prog_bar)
{
    ntg_prog_bar_deinit(_prog_bar);
}

void ntg_prog_bar_set_opts(ntg_prog_bar* prog_bar, const struct ntg_prog_bar_opts* opts)
{
    assert(prog_bar != NULL);

    struct ntg_prog_bar_opts old_opts = prog_bar->_opts;
    struct ntg_prog_bar_opts new_opts = (opts ? (*opts) : ntg_prog_bar_opts_def());

    if(ntg_prog_bar_opts_are_eq(&old_opts, &new_opts))
        return;

    prog_bar->_opts = new_opts;

    ntg_object_mark_dirty((ntg_object*)prog_bar, NTG_OBJECT_DIRTY_FULL);

    if(prog_bar->hooks.on_opts_chng_fn)
        prog_bar->hooks.on_opts_chng_fn(prog_bar, &old_opts, &new_opts);
}

void ntg_prog_bar_set_prog(ntg_prog_bar* prog_bar, double prog)
{
    assert(prog_bar != NULL);

    double old_prog = prog_bar->_prog;

    if(_double_are_eq(old_prog, prog))
        return;

    prog = _min2_double(1.0, prog);

    ntg_object_mark_dirty((ntg_object*)prog_bar, NTG_OBJECT_DIRTY_DRAW);

    if(prog_bar->hooks.on_prog_chng_fn)
        prog_bar->hooks.on_prog_chng_fn(prog_bar, old_prog, prog);
}

static struct ntg_object_measure measure_fn(
        const ntg_object* _prog_bar,
        ntg_orient orient,
        void* layout_ch,
        sarena* arena)
{
    const ntg_prog_bar* prog_bar = (const ntg_prog_bar*)_prog_bar;

    if(orient == prog_bar->_opts.orient)
    {
        return (struct ntg_object_measure) {
            .min_size = 10,
            .nat_size = 10,
            .max_size = SIZE_MAX,
            .grow = 1
        };
    }
    else
    {
        return (struct ntg_object_measure) {
            .min_size = 1,
            .nat_size = 1,
            .max_size = SIZE_MAX,
            .grow = 1
        };
    }
}

static void draw_fn(
        const ntg_object* _prog_bar,
        ntg_object_tmp_drawing* out_drawing,
        void* layout_ch,
        sarena* arena)
{
    const ntg_prog_bar* prog_bar = (const ntg_prog_bar*)_prog_bar;
    struct ntg_xy size = ntg_object_get_size_cont(_prog_bar);

    if(ntg_xy_is_zero(ntg_xy_size(size))) return;

    struct ntg_oxy _size = ntg_oxy_from_xy(size, prog_bar->_opts.orient);

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
                it_cell = prog_bar->_opts.style.complete_style;
            else if(complete_count == 0)
                it_cell = prog_bar->_opts.style.uncomplete_style;
            else
            {
                if(i < (complete_count - 1))
                    it_cell = prog_bar->_opts.style.complete_style;
                else if(i == (complete_count - 1))
                    it_cell = prog_bar->_opts.style.threshold_style;
                else
                    it_cell = prog_bar->_opts.style.uncomplete_style;
            }
            ntg_object_tmp_drawing_set(out_drawing, it_cell, it_xy);
        }
    }
}
