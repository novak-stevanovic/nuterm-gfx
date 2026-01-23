#include "ntg.h"
#include "shared/_ntg_shared.h"
#include <assert.h>
#include <stdlib.h>

struct ntg_decorator_ldata
{
    struct ntg_decorator_width width; // calculated width
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
        .mode = NTG_DECORATOR_ENABLE_ON_MIN,
        .width = ntg_decorator_width(1, 1, 1, 1)
    };
}

void ntg_decorator_set_opts(ntg_decorator* decorator, struct ntg_decorator_opts opts)
{
    assert(decorator != NULL);

    decorator->_opts = opts;

    ntg_entity_raise_event_((ntg_entity*)decorator, NTG_EVENT_OBJECT_DIFF, NULL);
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
        .init_fn = _ntg_decorator_layout_data_init_fn,
        .deinit_fn = _ntg_decorator_layout_data_deinit_fn
    };

    ntg_object_init((ntg_object*)decorator, object_data);

    decorator->_opts = ntg_decorator_opts_def();
}

void ntg_decorator_deinit(ntg_decorator* decorator)
{
    decorator->_opts = ntg_decorator_opts_def();

    if(decorator->_widget != NULL)
    {
        if(decorator->_widget->_padding == decorator)
            ntg_widget_set_padding(decorator->_widget, NULL);

        if(decorator->_widget->_border == decorator)
            ntg_widget_set_border(decorator->_widget, NULL);
    }

    ntg_object_deinit((ntg_object*)decorator);
}

void _ntg_decorator_decorate(ntg_decorator* decorator, ntg_widget* widget)
{
    assert(decorator != NULL);
    assert(decorator->_widget != widget);

    decorator->_widget = widget;
    int z_index = widget ? ntg_widget_get_z_index(widget) : 0;
    ntg_object_set_z_index((ntg_object*)decorator, z_index);
}

void* _ntg_decorator_layout_data_init_fn(const ntg_object* _decorator)
{
    struct ntg_decorator_ldata* layout_data;
    layout_data = malloc(sizeof(struct ntg_decorator_ldata));
    assert(layout_data != NULL);

    layout_data->width = (struct ntg_decorator_width) {0};

    return layout_data;
}

void _ntg_decorator_layout_data_deinit_fn(void* data, const ntg_object* _decorator)
{
    free(data);
}

struct ntg_object_measure 
_ntg_decorator_measure_fn(const ntg_object* _decorator, void* _layout_data,
                          ntg_orient orient, bool constrained, sarena* arena)
{
    const ntg_decorator* decorator = (ntg_decorator*)_decorator;
    if(_decorator->_children.size == 0) return (struct ntg_object_measure) {0};

    const ntg_object* child = _decorator->_children.data[0];
    struct ntg_object_measure child_data;
    // struct ntg_decorator_ldata* layout_data = _layout_data;
    child_data = ntg_object_get_measure(child, orient);

    if(orient == NTG_ORIENT_H)
    {
        size_t h_size = decorator->_opts.width.west + decorator->_opts.width.east;
        return (struct ntg_object_measure) {
            .min_size = child_data.min_size + h_size,
            .nat_size = child_data.nat_size + h_size,
            .max_size = (child_data.max_size == NTG_SIZE_MAX) ?
                NTG_SIZE_MAX : child_data.max_size + h_size,
            .grow = child->_grow.x
        };
    }
    else
    {
        size_t v_size = decorator->_opts.width.north + decorator->_opts.width.south;
        return (struct ntg_object_measure) {
            .min_size = child_data.min_size + v_size,
            .nat_size = child_data.nat_size + v_size,
            .max_size = (child_data.max_size == NTG_SIZE_MAX) ?
                NTG_SIZE_MAX : child_data.max_size + v_size,
            .grow = child->_grow.y
        };
    }
}

void _ntg_decorator_constrain_fn(
        const ntg_object* _decorator,
        void* _layout_data,
        ntg_orient orient,
        ntg_object_size_map* out_size_map,
        sarena* arena)
{
    const ntg_decorator* decorator = (ntg_decorator*)_decorator;
    const ntg_object* child = _decorator->_children.data[0];
    struct ntg_object_measure child_data;
    child_data = ntg_object_get_measure(child, orient);
    struct ntg_decorator_ldata* layout_data = _layout_data;
    size_t size = ntg_object_get_size_1d(_decorator, orient);

    size_t extra_space;
    if(orient == NTG_ORIENT_V)
    {
        size_t pref, final;
        pref = decorator->_opts.width.west + decorator->_opts.width.east;
        final = layout_data->width.west + layout_data->width.east;

        if((pref > 0) && (final == 0)) extra_space = 0;
        else extra_space = size;
    }
    else
    {
        if(decorator->_opts.mode == NTG_DECORATOR_ENABLE_ON_MIN)
            extra_space = _ssub_size(size, child_data.min_size);
        else if(decorator->_opts.mode == NTG_DECORATOR_ENABLE_ON_NAT)
            extra_space = _ssub_size(size, child_data.nat_size);
        else extra_space = size;
    }

    size_t _sizes[2] = {0};
    size_t caps[2];
    if(orient == NTG_ORIENT_H)
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

    if(orient == NTG_ORIENT_H)
    {
        layout_data->width.west = _sizes[0];
        layout_data->width.east = _sizes[1];
    }
    else
    {
        layout_data->width.north = _sizes[0];
        layout_data->width.south = _sizes[1];
    }

    size_t child_size = size - _sizes[0] - _sizes[1];

    ntg_object_size_map_set(out_size_map, child, child_size);
}

void _ntg_decorator_arrange_fn(
        const ntg_object* _decorator,
        void* _layout_data,
        ntg_object_xy_map* out_pos_map,
        sarena* arena)
{
    // const ntg_decorator* decorator = (ntg_decorator*)object;
    const ntg_object* child = _decorator->_children.data[0];
    struct ntg_decorator_ldata* layout_data = _layout_data;
    struct ntg_xy offset = ntg_xy(
            layout_data->width.west,
            layout_data->width.north);

    ntg_object_xy_map_set(out_pos_map, child, offset);
}
