#include "object/ntg_prog_bar.h"
#include "object/shared/ntg_object_drawing.h"
#include "shared/_ntg_shared.h"
#include <assert.h>
#include <math.h>

void __ntg_prog_bar_init__(ntg_prog_bar* prog_bar,
        ntg_orientation orientation, ntg_cell complete_cell,
        ntg_cell uncomplete_cell, ntg_cell threshold_cell,
        ntg_object_process_key_fn process_key_fn)
{
    assert(prog_bar != NULL);

    __ntg_object_init__(NTG_OBJECT(prog_bar),
            NTG_OBJECT_WIDGET,
            _ntg_prog_bar_measure_fn,
            NULL,
            NULL,
            _ntg_prog_bar_arrange_drawing_fn,
            process_key_fn);

    prog_bar->__orientation = orientation;
    prog_bar->__complete_cell = complete_cell;
    prog_bar->__threshold_cell = threshold_cell;
    prog_bar->__uncomplete_cell = uncomplete_cell;
    prog_bar->__percentage = 0;
}

void __ntg_prog_bar_deinit__(ntg_prog_bar* prog_bar)
{
    assert(prog_bar != NULL);

    __ntg_object_deinit__(NTG_OBJECT(prog_bar));

    prog_bar->__orientation = NTG_ORIENTATION_HORIZONTAL;
    prog_bar->__complete_cell = ntg_cell_default();
    prog_bar->__threshold_cell = ntg_cell_default();
    prog_bar->__uncomplete_cell = ntg_cell_default();
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

struct ntg_measure_result _ntg_prog_bar_measure_fn(const ntg_object* _prog_bar,
        ntg_orientation orientation, size_t for_size,
        const ntg_measure_context* context)
{
    assert(_prog_bar != NULL);

    ntg_prog_bar* prog_bar = NTG_PROG_BAR(_prog_bar);

    if(orientation == prog_bar->__orientation)
    {
        return (struct ntg_measure_result) {
            .min_size = 10,
            .natural_size = 10,
            .max_size = SIZE_MAX 
        };
    }
    else
    {
        return (struct ntg_measure_result) {
            .min_size = 1,
            .natural_size = 1,
            .max_size = SIZE_MAX 
        };
    }
}

void _ntg_prog_bar_arrange_drawing_fn(const ntg_object* _prog_bar,
        struct ntg_xy size, ntg_object_drawing* out_drawing)
{
    assert(_prog_bar != NULL);

    ntg_prog_bar* prog_bar = NTG_PROG_BAR(_prog_bar);

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
            _it_xy = ntg_oxy(i, j);
            it_xy = ntg_xy_from_oxy(_it_xy, prog_bar->__orientation);
            it_cell = ntg_object_drawing_at_(out_drawing, it_xy);
            if(complete_count == _size.prim_val)
                (*it_cell) = prog_bar->__complete_cell;
            else if(complete_count == 0)
                (*it_cell) = prog_bar->__uncomplete_cell;
            else
            {
                if(i < (complete_count - 1))
                    (*it_cell) = prog_bar->__complete_cell;
                else if(i == (complete_count - 1))
                    (*it_cell) = prog_bar->__threshold_cell;
                else
                    (*it_cell) = prog_bar->__uncomplete_cell;
            }
        }
    }
}
