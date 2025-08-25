#include <assert.h>
#include "object/ntg_border_box.h"
#include "shared/_ntg_shared.h"

static void __init_default(ntg_border_box* box)
{
    box->_north = NULL;
    box->_east = NULL;
    box->_south = NULL;
    box->_west = NULL;
    box->_center = NULL;
}

void __ntg_border_box_init__(ntg_border_box* box)
{
    assert(box != NULL);

    __ntg_object_init__(NTG_OBJECT(box), NTG_OBJECT_WIDGET,
            _ntg_border_box_measure_fn,
            _ntg_border_box_constrain_fn,
            _ntg_border_box_arrange_children_fn,
            NULL);

    __init_default(box);
}

void __ntg_border_box_deinit__(ntg_border_box* box)
{
    assert(box != NULL);

    __init_default(box);
}

struct ntg_measure_result _ntg_border_box_measure_fn(
        const ntg_object* _box,
        ntg_orientation orientation, size_t for_size,
        const ntg_measure_context* context)
{
    ntg_border_box* box = NTG_BORDER_BOX(_box);

    struct ntg_measure_data north_data = ntg_measure_context_get(
            context, box->_north);
    struct ntg_measure_data east_data = ntg_measure_context_get(
            context, box->_east);
    struct ntg_measure_data south_data = ntg_measure_context_get(
            context, box->_south);
    struct ntg_measure_data west_data = ntg_measure_context_get(
            context, box->_west);
    struct ntg_measure_data center_data = ntg_measure_context_get(
            context, box->_center);

    size_t min, natural, max;
    if(orientation == NTG_ORIENTATION_HORIZONTAL)
    {
        min = _max3_size(north_data.min_size,
                east_data.min_size + center_data.min_size + west_data.min_size,
                south_data.min_size);
        natural = _max3_size(north_data.natural_size,
                east_data.natural_size + center_data.natural_size + west_data.natural_size,
                south_data.natural_size);
        max = _max3_size(north_data.max_size,
                east_data.max_size + center_data.max_size + west_data.max_size,
                south_data.max_size);
    }
    else
    {
        min = _max3_size(north_data.min_size + west_data.min_size + south_data.min_size,
                north_data.min_size + center_data.min_size + south_data.min_size,
                north_data.min_size + east_data.min_size + south_data.min_size);
        natural = _max3_size(north_data.natural_size + west_data.natural_size + south_data.natural_size,
                north_data.natural_size + center_data.natural_size + south_data.natural_size,
                north_data.natural_size + east_data.natural_size + south_data.natural_size);
        max = _max3_size(north_data.max_size + west_data.max_size + south_data.max_size,
                north_data.max_size + center_data.max_size + south_data.max_size,
                north_data.max_size + east_data.max_size + south_data.max_size);
    }

    return (struct ntg_measure_result) {
        .min_size = min,
        .natural_size = natural,
        .max_size = max
    };
}

size_t _ntg_border_box_constrain_fn(
        const ntg_object* _box,
        ntg_orientation orientation, size_t size,
        const ntg_constrain_context* context,
        ntg_constrain_output* out_sizes)
{
    ntg_border_box* box = NTG_BORDER_BOX(_box);

    // TODO

    return 0;
}

void _ntg_border_box_arrange_children_fn(
        const ntg_object* _box,
        struct ntg_xy size,
        const ntg_arrange_context* context,
        ntg_arrange_output* out_positions)
{
    // TODO

    ntg_border_box* box = NTG_BORDER_BOX(_box);
}

void ntg_border_box_set_north(ntg_border_box* box, ntg_object* north)
{
    assert(box != NULL);

    if(box->_north != NULL)
        _ntg_object_rm_child(NTG_OBJECT(box), box->_north);

    ntg_object* group_root = ntg_object_get_group_root(north);
    ntg_object* parent = ntg_object_get_parent(north,
            NTG_OBJECT_GET_PARENT_EXCL_DECORATOR);

    assert(parent == NULL);

    _ntg_object_add_child(parent, group_root);

    box->_north = north;
}

void ntg_border_box_set_east(ntg_border_box* box, ntg_object* east)
{
    assert(box != NULL);

    if(box->_east != NULL)
        _ntg_object_rm_child(NTG_OBJECT(box), box->_east);

    ntg_object* group_root = ntg_object_get_group_root(east);
    ntg_object* parent = ntg_object_get_parent(east,
            NTG_OBJECT_GET_PARENT_EXCL_DECORATOR);

    assert(parent == NULL);

    _ntg_object_add_child(parent, group_root);

    box->_east = east;
}

void ntg_border_box_set_south(ntg_border_box* box, ntg_object* south)
{
    assert(box != NULL);

    if(box->_south != NULL)
        _ntg_object_rm_child(NTG_OBJECT(box), box->_south);

    ntg_object* group_root = ntg_object_get_group_root(south);
    ntg_object* parent = ntg_object_get_parent(south,
            NTG_OBJECT_GET_PARENT_EXCL_DECORATOR);

    assert(parent == NULL);

    _ntg_object_add_child(parent, group_root);

    box->_south = south;
}

void ntg_border_box_set_west(ntg_border_box* box, ntg_object* west)
{
    assert(box != NULL);

    if(box->_west != NULL)
        _ntg_object_rm_child(NTG_OBJECT(box), box->_west);

    ntg_object* group_root = ntg_object_get_group_root(west);
    ntg_object* parent = ntg_object_get_parent(west,
            NTG_OBJECT_GET_PARENT_EXCL_DECORATOR);

    assert(parent == NULL);

    _ntg_object_add_child(parent, group_root);

    box->_west = west;
}

void ntg_border_box_set_center(ntg_border_box* box, ntg_object* center)
{
    assert(box != NULL);

    if(box->_center != NULL)
        _ntg_object_rm_child(NTG_OBJECT(box), box->_center);

    ntg_object* group_root = ntg_object_get_group_root(center);
    ntg_object* parent = ntg_object_get_parent(center,
            NTG_OBJECT_GET_PARENT_EXCL_DECORATOR);

    assert(parent == NULL);

    _ntg_object_add_child(parent, group_root);

    box->_center = center;
}
