#include "ntg.h"
#include "shared/_ntg_shared.h"
#include <assert.h>
#include <stdlib.h>

struct ntg_decorator_ldata
{
    struct ntg_decorator_width final_width; // determined in constrain phase
};

struct ntg_decorator_width ntg_decorator_width(
        size_t north, size_t east,
        size_t south, size_t west)
{
    return (struct ntg_decorator_width) {
        .north = north,
        .east = east,
        .south = south,
        .west = west
    };
}

struct ntg_decorator_opts ntg_decorator_opts_def()
{
    return (struct ntg_decorator_opts) {
        .mode = NTG_PADDING_ENABLE_ON_NATURAL,
        .width = ntg_decorator_width(1, 1, 1, 1)
    };
}

void ntg_decorator_set_opts(ntg_decorator* decorator, struct ntg_decorator_opts opts)
{
    assert(decorator != NULL);

    decorator->_opts = opts;

    ntg_entity_raise_event((ntg_entity*)decorator, NULL, NTG_EVENT_OBJECT_DIFF, NULL);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_decorator_init(ntg_decorator* decorator, ntg_object_draw_fn draw_fn)
{
    assert(decorator != NULL);
    assert(draw_fn != NULL);

    struct ntg_object_layout_ops object_data = {
        .measure_fn = _ntg_decorator_measure_fn,
        .constrain_fn = _ntg_decorator_constrain_fn,
        .arrange_fn = _ntg_decorator_arrange_fn,
        .draw_fn = draw_fn,
    };

    _ntg_object_init((ntg_object*)decorator, object_data);

    struct ntg_decorator_ldata* data = malloc(sizeof(struct ntg_decorator_ldata));
    assert(data != NULL);
    (*data) = (struct ntg_decorator_ldata) {0};
    ((ntg_object*)decorator)->layout_data = data;

    decorator->_opts = ntg_decorator_opts_def();
}

void _ntg_decorator_deinit_fn(ntg_entity* entity)
{
    assert(entity != NULL);

    ntg_decorator* decorator = (ntg_decorator*)entity;
    decorator->_opts = ntg_decorator_opts_def();
    free(((ntg_object*)decorator)->layout_data);

    _ntg_object_deinit_fn(entity);
}

struct ntg_object_measure _ntg_decorator_measure_fn(
        const ntg_object* _decorator,
        void* _layout_data,
        ntg_orientation orientation,
        size_t for_size,
        sarena* arena)
{
    const ntg_decorator* decorator = (ntg_decorator*)_decorator;
    const ntg_object* child = ntg_object_get_children(_decorator)->data[0];
    struct ntg_object_measure child_data = ntg_object_get_measure(child,
            orientation, NTG_OBJECT_SELF);

    if(orientation == NTG_ORIENT_H)
    {
        size_t h_size = decorator->_opts.width.west + decorator->_opts.width.east;
        return (struct ntg_object_measure) {
            .min_size = child_data.min_size + h_size,
            .natural_size = child_data.natural_size + h_size,
            .max_size = (child_data.max_size == NTG_SIZE_MAX) ?
                NTG_SIZE_MAX : child_data.max_size + h_size,
            .grow = 1
        };
    }
    else
    {
        size_t v_size = decorator->_opts.width.north + decorator->_opts.width.south;
        return (struct ntg_object_measure) {
            .min_size = child_data.min_size + v_size,
            .natural_size = child_data.natural_size + v_size,
            .max_size = (child_data.max_size == NTG_SIZE_MAX) ?
                NTG_SIZE_MAX : child_data.max_size + v_size,
            .grow = 1
        };
    }
}

void _ntg_decorator_constrain_fn(
        const ntg_object* _decorator,
        void* _layout_data,
        ntg_orientation orientation,
        size_t size,
        ntg_object_size_map* out_sizes,
        sarena* arena)
{
    const ntg_decorator* decorator = (ntg_decorator*)_decorator;
    const ntg_object* child = ntg_object_get_children(_decorator)->data[0];
    struct ntg_object_measure child_data = ntg_object_get_measure(child,
            orientation, NTG_OBJECT_SELF);
    struct ntg_decorator_ldata* layout_data = (struct ntg_decorator_ldata*)_layout_data;

    size_t extra_space;
    if(decorator->_opts.mode == NTG_PADDING_ENABLE_ON_NATURAL)
        extra_space = _ssub_size(size, child_data.natural_size);
    else if(decorator->_opts.mode == NTG_PADDING_ENABLE_ON_MIN)
        extra_space = _ssub_size(size, child_data.min_size);
    else if(decorator->_opts.mode == NTG_PADDING_ENABLE_ALWAYS)
        extra_space = size;
    else assert(0);

    size_t _sizes[2] = {0};
    size_t caps[2];
    if(orientation == NTG_ORIENT_H)
    {
        caps[0] = decorator->_opts.width.west;
        caps[1] = decorator->_opts.width.east;
    }
    else
    {
        caps[0] = decorator->_opts.width.north;
        caps[1] = decorator->_opts.width.south;
    }

    ntg_sap_cap_round_robin(caps, NULL, _sizes, extra_space, 2, arena);

    if(orientation == NTG_ORIENT_H)
    {
        layout_data->final_width.west = _sizes[0];
        layout_data->final_width.east = _sizes[1];
    }
    else
    {
        layout_data->final_width.north = _sizes[0];
        layout_data->final_width.south = _sizes[1];
    }

    size_t child_size = size - _sizes[0] - _sizes[1];

    ntg_object_size_map_set(out_sizes, child, child_size, NTG_OBJECT_SELF);
}

void _ntg_decorator_arrange_fn(
        const ntg_object* _decorator,
        void* _layout_data,
        struct ntg_xy size,
        ntg_object_xy_map* out_positions,
        sarena* arena)
{
    // const ntg_decorator* decorator = (ntg_decorator*)object;
    const ntg_object* child = ntg_object_get_children(_decorator)->data[0];
    struct ntg_decorator_ldata* layout_data = _layout_data;
    struct ntg_xy offset = ntg_xy(
            layout_data->final_width.west,
            layout_data->final_width.north);

    ntg_object_xy_map_set(out_positions, child, offset, NTG_OBJECT_SELF);
}
