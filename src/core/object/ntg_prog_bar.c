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

struct ntg_prog_bar_opts ntg_prog_bar_opts_def()
{
    return (struct ntg_prog_bar_opts) {
        .orient = NTG_ORIENT_H,
        .complete_style = ntg_vcell_bg(nt_color_new_auto(nt_rgb_new(0, 255, 0))),
        .uncomplete_style = ntg_vcell_bg(nt_color_new_auto(nt_rgb_new(255, 0, 0))),
        .threshold_style = ntg_vcell_bg(nt_color_new_auto(nt_rgb_new(0, 255, 0))),
    };
}

void ntg_prog_bar_init(ntg_prog_bar* prog_bar)
{
    assert(prog_bar != NULL);

    struct ntg_object_layout_ops object_data = {
        .measure_fn = measure_fn,
        .constrain_fn = NULL,
        .arrange_fn = NULL,
        .draw_fn = draw_fn,
        .init_fn = NULL,
        .deinit_fn = NULL
    };

    struct ntg_object_hooks hooks = {0};

    ntg_object_init((ntg_object*)prog_bar, object_data, hooks, &NTG_TYPE_PROG_BAR);

    prog_bar->_opts = ntg_prog_bar_opts_def();
}

void ntg_prog_bar_set_opts(ntg_prog_bar* prog_bar, struct ntg_prog_bar_opts opts)
{
    assert(prog_bar != NULL);

    opts.percentage = _min2_double(1.0, opts.percentage);

    prog_bar->_opts = opts;

    ntg_object_add_dirty((ntg_object*)prog_bar, NTG_OBJECT_DIRTY_FULL);
}

void ntg_prog_bar_deinit(ntg_prog_bar* prog_bar)
{
    prog_bar->_opts = ntg_prog_bar_opts_def();

    ntg_object_deinit((ntg_object*)prog_bar);
}

void ntg_prog_bar_deinit_(void* _prog_bar)
{
    ntg_prog_bar_deinit(_prog_bar);
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

    size_t complete_count = round(_size.prim_val * prog_bar->_opts.percentage);

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
                it_cell = prog_bar->_opts.complete_style;
            else if(complete_count == 0)
                it_cell = prog_bar->_opts.uncomplete_style;
            else
            {
                if(i < (complete_count - 1))
                    it_cell = prog_bar->_opts.complete_style;
                else if(i == (complete_count - 1))
                    it_cell = prog_bar->_opts.threshold_style;
                else
                    it_cell = prog_bar->_opts.uncomplete_style;
            }
            ntg_object_tmp_drawing_set(out_drawing, it_cell, it_xy);
        }
    }
}
