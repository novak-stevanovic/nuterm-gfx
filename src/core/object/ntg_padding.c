#include "ntg.h"
#include "shared/_ntg_shared.h"
#include <assert.h>
#include <stdlib.h>

struct ntg_padding_width ntg_padding_width(size_t north,
        size_t east, size_t south, size_t west)
{
    return (struct ntg_padding_width) {
        .north = north,
        .east = east,
        .south = south,
        .west = west
    };
}

struct ntg_padding_opts ntg_padding_opts_def()
{
    return (struct ntg_padding_opts) {
        .mode = NTG_PADDING_ENABLE_ON_NATURAL,
        .width = ntg_padding_width(1, 1, 1, 1)
    };
}

void ntg_padding_set_opts(ntg_padding* padding, struct ntg_padding_opts opts)
{
    assert(padding != NULL);

    padding->_opts = opts;

    _ntg_object_mark_change((ntg_object*)padding);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_padding_init(ntg_padding* padding, ntg_object_draw_fn draw_fn)
{
    assert(padding != NULL);
    assert(draw_fn != NULL);

    struct ntg_object_layout_ops object_data = {
        .init_fn = ntg_padding_layout_initfn,
        .deinit_fn = _ntg_padding_layout_deinit_fn,
        .measure_fn = _ntg_padding_measure_fn,
        .constrain_fn = _ntg_padding_constrain_fn,
        .arrange_fn = _ntg_padding_arrange_fn,
        .draw_fn = draw_fn,
    };

    _ntg_object_init((ntg_object*)padding, object_data);

    padding->_opts = ntg_padding_opts_def();
}

void _ntg_padding_deinit_fn(ntg_entity* entity)
{
    assert(entity != NULL);

    ntg_padding* padding = (ntg_padding*)entity;
    padding->_opts = ntg_padding_opts_def();

    _ntg_object_deinit_fn(entity);
}

void* ntg_padding_layout_initfn(const ntg_object* object)
{
    struct ntg_padding_ldata* new = (struct ntg_padding_ldata*)malloc(
            sizeof(struct ntg_padding_ldata));
    assert(new != NULL);

    (*new) = (struct ntg_padding_ldata) {0};

    return new;
}

void _ntg_padding_layout_deinit_fn(const ntg_object* object, void* layout_data)
{
    free(layout_data);
}

struct ntg_object_measure _ntg_padding_measure_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        struct ntg_object_measure_ctx ctx,
        struct ntg_object_measure_out* out,
        void* _layout_data,
        sarena* arena)
{
    const ntg_padding* padding = (ntg_padding*)object;
    const ntg_object* child = ntg_object_get_children(object).data[0];
    struct ntg_object_measure child_data = ntg_object_measure_map_get(
            ctx.measures, child);

    if(orientation == NTG_ORIENT_H)
    {
        size_t h_size = padding->_opts.width.west + padding->_opts.width.east;
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
        size_t v_size = padding->_opts.width.north + padding->_opts.width.south;
        return (struct ntg_object_measure) {
            .min_size = child_data.min_size + v_size,
            .natural_size = child_data.natural_size + v_size,
            .max_size = (child_data.max_size == NTG_SIZE_MAX) ?
                NTG_SIZE_MAX : child_data.max_size + v_size,
            .grow = 1
        };
    }
}

void _ntg_padding_constrain_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t size,
        struct ntg_object_constrain_ctx ctx,
        struct ntg_object_constrain_out* out,
        void* _layout_data,
        sarena* arena)
{
    const ntg_padding* padding = (ntg_padding*)object;
    const ntg_object* child = ntg_object_get_children(object).data[0];
    struct ntg_object_measure child_data = ntg_object_measure_map_get(
            ctx.measures, child);
    struct ntg_padding_ldata* layout_data = (struct ntg_padding_ldata*)_layout_data;

    size_t extra_space;
    if(padding->_opts.mode == NTG_PADDING_ENABLE_ON_NATURAL)
        extra_space = _ssub_size(size, child_data.natural_size);
    else if(padding->_opts.mode == NTG_PADDING_ENABLE_ON_MIN)
        extra_space = _ssub_size(size, child_data.min_size);
    else if(padding->_opts.mode == NTG_PADDING_ENABLE_ALWAYS)
        extra_space = size;
    else assert(0);

    size_t _sizes[2] = {0};
    size_t caps[2];
    if(orientation == NTG_ORIENT_H)
    {
        caps[0] = padding->_opts.width.west;
        caps[1] = padding->_opts.width.east;
    }
    else
    {
        caps[0] = padding->_opts.width.north;
        caps[1] = padding->_opts.width.south;
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

    ntg_object_size_map_set(out->sizes, child, child_size);
}

void _ntg_padding_arrange_fn(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_arrange_ctx ctx,
        struct ntg_object_arrange_out* out,
        void* _layout_data,
        sarena* arena)
{
    // const ntg_padding* padding = (ntg_padding*)object;
    const ntg_object* child = ntg_object_get_children(object).data[0];
    // struct ntg_xy child_size = ntg_object_xy_map_get(ctx.sizes, child);
    struct ntg_padding_ldata* layout_data = (struct ntg_padding_ldata*)_layout_data;
    struct ntg_xy offset = ntg_xy(
            layout_data->final_width.west,
            layout_data->final_width.north);

    ntg_object_xy_map_set(out->positions, child, offset);
}
