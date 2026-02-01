#include "ntg.h"
#include <assert.h>
#include <math.h>
#include "shared/ntg_shared_internal.h"

static struct ntg_object_measure measure_fn(
        const ntg_object* _pbar,
        ntg_orient orient,
        void* lctx,
        sarena* arena);

static void draw_fn(
        const ntg_object* _pbar,
        ntg_object_tmp_drawing* out_drawing,
        void* lctx,
        sarena* arena);

struct ntg_pbar_opts ntg_pbar_opts_def()
{
    return (struct ntg_pbar_opts) {
        .orient = NTG_ORIENT_H,
        .complete_style = ntg_vcell_bg(nt_color_new_auto(nt_rgb_new(0, 255, 0))),
        .uncomplete_style = ntg_vcell_bg(nt_color_new_auto(nt_rgb_new(255, 0, 0))),
        .threshold_style = ntg_vcell_bg(nt_color_new_auto(nt_rgb_new(0, 255, 0))),
    };
}

void ntg_pbar_init(ntg_pbar* pbar)
{
    assert(pbar != NULL);

    struct ntg_object_layout_ops object_data = {
        .measure_fn = measure_fn,
        .constrain_fn = NULL,
        .arrange_fn = NULL,
        .draw_fn = draw_fn,
        .init_fn = NULL,
        .deinit_fn = NULL
    };

    struct ntg_object_hooks hooks = {0};

    ntg_object_init((ntg_object*)pbar, object_data, hooks, &NTG_TYPE_PBAR);

    pbar->_opts = ntg_pbar_opts_def();
}

void ntg_pbar_set_opts(ntg_pbar* pbar, struct ntg_pbar_opts opts)
{
    assert(pbar != NULL);

    opts.percentage = _min2_double(1.0, opts.percentage);

    pbar->_opts = opts;

    ntg_object_add_dirty((ntg_object*)pbar, NTG_OBJECT_DIRTY_FULL);
}

void ntg_pbar_deinit(ntg_pbar* pbar)
{
    pbar->_opts = ntg_pbar_opts_def();

    ntg_object_deinit((ntg_object*)pbar);
}

static struct ntg_object_measure measure_fn(
        const ntg_object* _pbar,
        ntg_orient orient,
        void* lctx,
        sarena* arena)
{
    const ntg_pbar* pbar = (const ntg_pbar*)_pbar;

    if(orient == pbar->_opts.orient)
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
        const ntg_object* _pbar,
        ntg_object_tmp_drawing* out_drawing,
        void* lctx,
        sarena* arena)
{
    const ntg_pbar* pbar = (const ntg_pbar*)_pbar;
    struct ntg_xy size = ntg_object_get_size_cont(_pbar);

    if(ntg_xy_is_zero(ntg_xy_size(size))) return;

    struct ntg_oxy _size = ntg_oxy_from_xy(size, pbar->_opts.orient);

    size_t complete_count = round(_size.prim_val * pbar->_opts.percentage);

    size_t i, j;
    struct ntg_oxy _it_xy;
    struct ntg_xy it_xy;
    struct ntg_vcell it_cell;
    for(i = 0; i < _size.prim_val; i++)
    {
        for(j = 0; j < _size.sec_val; j++)
        {
            _it_xy = ntg_oxy(i, j, pbar->_opts.orient);
            it_xy = ntg_xy_from_oxy(_it_xy);
            if(complete_count == _size.prim_val)
                it_cell = pbar->_opts.complete_style;
            else if(complete_count == 0)
                it_cell = pbar->_opts.uncomplete_style;
            else
            {
                if(i < (complete_count - 1))
                    it_cell = pbar->_opts.complete_style;
                else if(i == (complete_count - 1))
                    it_cell = pbar->_opts.threshold_style;
                else
                    it_cell = pbar->_opts.uncomplete_style;
            }
            ntg_object_tmp_drawing_set(out_drawing, it_cell, it_xy);
        }
    }
}
