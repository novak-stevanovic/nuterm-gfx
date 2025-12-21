#include <assert.h>
#include <math.h>

#include "core/object/ntg_prog_bar.h"
#include "core/object/shared/ntg_object_drawing.h"
#include "core/object/shared/ntg_object_measure.h"
#include "core/object/shared/ntg_object_types.h"
#include "shared/_ntg_shared.h"

void _ntg_prog_bar_init_(
        ntg_prog_bar* prog_bar,
        ntg_orientation orientation,
        struct ntg_prog_bar_style style,
        struct ntg_object_event_ops event_ops,
        ntg_object_deinit_fn deinit_fn,
        void* data,
        ntg_object_container* container)
{
    assert(prog_bar != NULL);

    struct ntg_object_layout_ops layout_ops = {
        .layout_init_fn = NULL,
        .layout_deinit_fn = NULL,
        .measure_fn = _ntg_prog_bar_measure_fn,
        .constrain_fn = NULL,
        .arrange_fn = NULL,
        .draw_fn = _ntg_prog_bar_draw_fn
    };

    _ntg_object_init_((ntg_object*)prog_bar,
            NTG_OBJECT_PROG_BAR,
            layout_ops,
            event_ops,
            (deinit_fn != NULL) ? deinit_fn : _ntg_prog_bar_deinit_fn,
            data,
            container);

    prog_bar->__orientation = orientation;
    prog_bar->__style = style;
    prog_bar->__percentage = 0;
}

void ntg_prog_bar_set_percentage(ntg_prog_bar* prog_bar, double percentage)
{
    assert(prog_bar != NULL);

    prog_bar->__percentage = _min2_double(1.0, percentage);
}

double ntg_prog_bar_get_percentage(const ntg_prog_bar* prog_bar)
{
    assert(prog_bar != NULL);

    return prog_bar->__percentage;
}

void _ntg_prog_bar_deinit_fn(ntg_object* object)
{
    assert(object != NULL);

    ntg_prog_bar* prog_bar = (ntg_prog_bar*)object;

    _ntg_object_deinit_fn((ntg_object*)prog_bar);

    prog_bar->__orientation = NTG_ORIENTATION_H;
    prog_bar->__style.complete_cell = ntg_cell_default();
    prog_bar->__style.threshold_cell = ntg_cell_default();
    prog_bar->__style.uncomplete_cell = ntg_cell_default();
    prog_bar->__percentage = 0;
}

struct ntg_object_measure _ntg_prog_bar_measure_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        struct ntg_object_measure_ctx ctx,
        struct ntg_object_measure_out* out,
        void* layout_data,
        sarena* arena)
{
    const ntg_prog_bar* prog_bar = (const ntg_prog_bar*)object;

    if(orientation == prog_bar->__orientation)
    {
        return (struct ntg_object_measure) {
            .min_size = 10,
            .natural_size = 10,
            .max_size = SIZE_MAX 
        };
    }
    else
    {
        return (struct ntg_object_measure) {
            .min_size = 1,
            .natural_size = 1,
            .max_size = SIZE_MAX 
        };
    }
}

void _ntg_prog_bar_draw_fn(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_draw_ctx ctx,
        struct ntg_object_draw_out* out,
        void* layout_data,
        sarena* arena)
{
    const ntg_prog_bar* prog_bar = (const ntg_prog_bar*)object;

    if(ntg_xy_is_zero(ntg_xy_size(size))) return;

    struct ntg_oxy _size = ntg_oxy_from_xy(size, prog_bar->__orientation);

    size_t complete_count = round(_size.prim_val * prog_bar->__percentage);

    size_t i, j;
    struct ntg_oxy _it_xy;
    struct ntg_xy it_xy;
    ntg_cell* it_cell;
    for(i = 0; i < _size.prim_val; i++)
    {
        for(j = 0; j < _size.sec_val; j++)
        {
            _it_xy = ntg_oxy(i, j, prog_bar->__orientation);
            it_xy = ntg_xy_from_oxy(_it_xy);
            it_cell = ntg_object_drawing_at_(out->drawing, it_xy);
            if(complete_count == _size.prim_val)
                (*it_cell) = prog_bar->__style.complete_cell;
            else if(complete_count == 0)
                (*it_cell) = prog_bar->__style.uncomplete_cell;
            else
            {
                if(i < (complete_count - 1))
                    (*it_cell) = prog_bar->__style.complete_cell;
                else if(i == (complete_count - 1))
                    (*it_cell) = prog_bar->__style.threshold_cell;
                else
                    (*it_cell) = prog_bar->__style.uncomplete_cell;
            }
        }
    }
}
