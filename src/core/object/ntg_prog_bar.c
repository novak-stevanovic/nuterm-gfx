#include "ntg.h"
#include "core/object/ntg_prog_bar.h"
#include "base/entity/ntg_entity_type.h"
#include "core/object/shared/ntg_object_drawing.h"
#include "core/object/shared/ntg_object_measure.h"
#include "shared/_ntg_shared.h"

struct ntg_prog_bar_opts ntg_prog_bar_opts_def()
{
    return (struct ntg_prog_bar_opts) {
        .orientation = NTG_ORIENTATION_H,
        .complete_style = ntg_vcell_bg(nt_color_new_auto(nt_rgb_new(0, 255, 0))),
        .uncomplete_style = ntg_vcell_bg(nt_color_new_auto(nt_rgb_new(255, 0, 0))),
        .threshold_style = ntg_vcell_bg(nt_color_new_auto(nt_rgb_new(0, 255, 0))),
    };
}

ntg_prog_bar* ntg_prog_bar_new(ntg_entity_system* system)
{
    struct ntg_entity_init_data entity_data = {
        .type = &NTG_ENTITY_PROG_BAR,
        .deinit_fn = _ntg_prog_bar_deinit_fn,
        .system = system
    };

    ntg_prog_bar* new = (ntg_prog_bar*)ntg_entity_create(entity_data);
    assert(new != NULL);

    return new;
}

void _ntg_prog_bar_init_(ntg_prog_bar* prog_bar)
{
    assert(prog_bar != NULL);

    struct ntg_object_layout_ops object_data = {
        .init_fn = NULL,
        .deinit_fn = NULL,
        .measure_fn = _ntg_prog_bar_measure_fn,
        .constrain_fn = NULL,
        .arrange_fn = NULL,
        .draw_fn = _ntg_prog_bar_draw_fn,
    };

    _ntg_object_init_((ntg_object*)prog_bar, object_data);

    prog_bar->__opts = ntg_prog_bar_opts_def();
}

void ntg_prog_bar_set_percentage(ntg_prog_bar* prog_bar, double percentage)
{
    assert(prog_bar != NULL);

    prog_bar->__percentage = _min2_double(1.0, percentage);

    _ntg_object_mark_change((ntg_object*)prog_bar);
}

void _ntg_prog_bar_deinit_fn(ntg_entity* entity)
{
    ntg_prog_bar* prog_bar = (ntg_prog_bar*)entity;
    prog_bar->__opts = ntg_prog_bar_opts_def();

    _ntg_object_deinit_fn(entity);
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

    if(orientation == prog_bar->__opts.orientation)
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

    struct ntg_oxy _size = ntg_oxy_from_xy(size, prog_bar->__opts.orientation);

    size_t complete_count = round(_size.prim_val * prog_bar->__percentage);

    size_t i, j;
    struct ntg_oxy _it_xy;
    struct ntg_xy it_xy;
    struct ntg_vcell* it_cell;
    for(i = 0; i < _size.prim_val; i++)
    {
        for(j = 0; j < _size.sec_val; j++)
        {
            _it_xy = ntg_oxy(i, j, prog_bar->__opts.orientation);
            it_xy = ntg_xy_from_oxy(_it_xy);
            it_cell = ntg_object_drawing_at_(out->drawing, it_xy);
            if(complete_count == _size.prim_val)
                (*it_cell) = prog_bar->__opts.complete_style;
            else if(complete_count == 0)
                (*it_cell) = prog_bar->__opts.uncomplete_style;
            else
            {
                if(i < (complete_count - 1))
                    (*it_cell) = prog_bar->__opts.complete_style;
                else if(i == (complete_count - 1))
                    (*it_cell) = prog_bar->__opts.threshold_style;
                else
                    (*it_cell) = prog_bar->__opts.uncomplete_style;
            }
        }
    }
}
