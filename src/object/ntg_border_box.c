#include <assert.h>
#include "object/ntg_border_box.h"
#include "base/ntg_sap.h"
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

    struct ntg_measure_data north_data = 
        (box->_north != NULL) ?
        ntg_measure_context_get(context, box->_north) :
        (struct ntg_measure_data) {0};
    struct ntg_measure_data east_data = 
        (box->_east != NULL) ?
        ntg_measure_context_get(context, box->_east) :
        (struct ntg_measure_data) {0};
    struct ntg_measure_data south_data = 
        (box->_south != NULL) ?
        ntg_measure_context_get(context, box->_south) :
        (struct ntg_measure_data) {0};
    struct ntg_measure_data west_data = 
        (box->_west != NULL) ?
        ntg_measure_context_get(context, box->_west) :
        (struct ntg_measure_data) {0};
    struct ntg_measure_data center_data = 
        (box->_center != NULL) ?
        ntg_measure_context_get(context, box->_center) :
        (struct ntg_measure_data) {0};

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

void _ntg_border_box_constrain_fn(
        const ntg_object* _box,
        ntg_orientation orientation, size_t size,
        const ntg_constrain_context* context,
        ntg_constrain_output* out_sizes)
{
    ntg_border_box* box = NTG_BORDER_BOX(_box);

    struct ntg_constrain_data north_data = 
        (box->_north != NULL) ?
        ntg_constrain_context_get(context, box->_north) :
        (struct ntg_constrain_data) {0};
    struct ntg_constrain_data east_data = 
        (box->_east != NULL) ?
        ntg_constrain_context_get(context, box->_east) :
        (struct ntg_constrain_data) {0};
    struct ntg_constrain_data south_data = 
        (box->_south != NULL) ?
        ntg_constrain_context_get(context, box->_south) :
        (struct ntg_constrain_data) {0};
    struct ntg_constrain_data west_data = 
        (box->_west != NULL) ?
        ntg_constrain_context_get(context, box->_west) :
        (struct ntg_constrain_data) {0};
    struct ntg_constrain_data center_data = 
        (box->_center != NULL) ?
        ntg_constrain_context_get(context, box->_center) :
        (struct ntg_constrain_data) {0};

    // size_t min_size = ntg_constrain_context_get_min_size(context);
    // size_t natural_size = ntg_constrain_context_get_natural_size(context);
    // size_t max_size = ntg_constrain_context_get_max_size(context);

    size_t caps[3] = {0};
    size_t _sizes[3] = {0};
    size_t extra_size = 0;

    if(orientation == NTG_ORIENTATION_HORIZONTAL)
    {
        struct ntg_constrain_result north_south_result = { .size = size };
        if(box->_north != NULL)
            ntg_constrain_output_set(out_sizes, box->_north, north_south_result);
        if(box->_south != NULL)
            ntg_constrain_output_set(out_sizes, box->_south, north_south_result);

        struct ntg_constrain_data wce_data = {
            .min_size = west_data.min_size + center_data.min_size + east_data.min_size,
            .natural_size = west_data.natural_size + center_data.natural_size + east_data.natural_size,
            .max_size = west_data.max_size + center_data.max_size + east_data.max_size,
        };

        if(size >= wce_data.natural_size)
        {
            caps[0] = west_data.max_size;
            caps[1] = center_data.max_size;
            caps[2] = east_data.max_size;
            _sizes[0] = west_data.natural_size;
            _sizes[1] = center_data.natural_size;
            _sizes[2] = east_data.natural_size;
            extra_size = size - wce_data.natural_size;
        }
        else if(size > wce_data.min_size)
        {
            caps[0] = west_data.natural_size;
            caps[1] = center_data.natural_size;
            caps[2] = east_data.natural_size;
            _sizes[0] = west_data.min_size;
            _sizes[1] = center_data.min_size;
            _sizes[2] = east_data.min_size;
            extra_size = size - wce_data.min_size;
        }
        else // size < wce_data.min-size
        {
            caps[0] = west_data.min_size;
            caps[1] = center_data.min_size;
            caps[2] = east_data.min_size;
            _sizes[0] = 0;
            _sizes[1] = 0;
            _sizes[2] = 0;
            extra_size = size;
        }
        ntg_sap_cap_round_robin(caps, _sizes, extra_size, 3);

        struct ntg_constrain_result west_result = { .size = _sizes[0] };
        struct ntg_constrain_result center_result = { .size = _sizes[1] };
        struct ntg_constrain_result east_result = { .size = _sizes[2] };

        if(box->_west != NULL)
            ntg_constrain_output_set(out_sizes, box->_west, west_result);
        if(box->_center != NULL)
            ntg_constrain_output_set(out_sizes, box->_center, center_result);
        if(box->_east != NULL)
            ntg_constrain_output_set(out_sizes, box->_east, east_result);
    }
    else // NTG_ORIENTATION_VERTICAL
    {
        struct ntg_constrain_data wce_data = {
            .min_size = _max3_size(west_data.min_size, center_data.min_size, east_data.min_size),
            .natural_size = _max3_size(west_data.natural_size, center_data.natural_size, east_data.natural_size),
            .max_size = _max3_size(west_data.max_size, center_data.max_size, east_data.max_size)
        };

        struct ntg_constrain_data total_data = {
            .min_size = wce_data.min_size + north_data.min_size + south_data.min_size,
            .natural_size = wce_data.natural_size + north_data.natural_size + south_data.natural_size,
            .max_size = wce_data.max_size + north_data.max_size + south_data.max_size
        };

        if(size >= total_data.natural_size)
        {
            caps[0] = north_data.max_size;
            caps[1] = wce_data.max_size;
            caps[2] = south_data.max_size;
            _sizes[0] = north_data.natural_size;
            _sizes[1] = wce_data.natural_size;
            _sizes[2] = south_data.natural_size;
            extra_size = size - total_data.natural_size;
        }
        else if(size >= total_data.min_size)
        {
            caps[0] = north_data.natural_size;
            caps[1] = wce_data.natural_size;
            caps[2] = south_data.natural_size;
            _sizes[0] = north_data.min_size;
            _sizes[1] = wce_data.min_size;
            _sizes[2] = south_data.min_size;
            extra_size = size - total_data.min_size;
        }
        else
        {
            caps[0] = north_data.min_size;
            caps[1] = wce_data.min_size;
            caps[2] = south_data.min_size;
            _sizes[0] = 0;
            _sizes[1] = 0;
            _sizes[2] = 0;
            extra_size = size;
        }

        ntg_sap_cap_round_robin(caps, _sizes, extra_size, 3);

        struct ntg_constrain_result north_result = { .size = _sizes[0] };
        struct ntg_constrain_result wce_result = { .size = _sizes[1] };
        struct ntg_constrain_result south_result = { .size = _sizes[2] };

        if(box->_north != NULL)
            ntg_constrain_output_set(out_sizes, box->_north, north_result);
        if(box->_east != NULL)
            ntg_constrain_output_set(out_sizes, box->_east, wce_result);
        if(box->_center != NULL)
            ntg_constrain_output_set(out_sizes, box->_center, wce_result);
        if(box->_west != NULL)
            ntg_constrain_output_set(out_sizes, box->_west, wce_result);
        if(box->_south != NULL)
            ntg_constrain_output_set(out_sizes, box->_center, south_result);
    }
}

void _ntg_border_box_arrange_children_fn(
        const ntg_object* _box,
        struct ntg_xy size,
        const ntg_arrange_context* context,
        ntg_arrange_output* out_positions)
{
    ntg_border_box* box = NTG_BORDER_BOX(_box);

    struct ntg_arrange_data north_data = 
        (box->_north != NULL) ?
        ntg_arrange_context_get(context, box->_north) :
        (struct ntg_arrange_data) {0};
    struct ntg_arrange_data east_data = 
        (box->_east != NULL) ?
        ntg_arrange_context_get(context, box->_east) :
        (struct ntg_arrange_data) {0};
    struct ntg_arrange_data south_data = 
        (box->_south != NULL) ?
        ntg_arrange_context_get(context, box->_south) :
        (struct ntg_arrange_data) {0};
    struct ntg_arrange_data west_data = 
        (box->_west != NULL) ?
        ntg_arrange_context_get(context, box->_west) :
        (struct ntg_arrange_data) {0};
    struct ntg_arrange_data center_data = 
        (box->_center != NULL) ?
        ntg_arrange_context_get(context, box->_center) :
        (struct ntg_arrange_data) {0};

    struct ntg_xy north_size = ntg_xy_size(north_data.size);
    struct ntg_xy east_size = ntg_xy_size(east_data.size);
    struct ntg_xy south_size = ntg_xy_size(south_data.size);
    struct ntg_xy west_size = ntg_xy_size(west_data.size);
    struct ntg_xy center_size = ntg_xy_size(center_data.size);

    struct ntg_xy north_pos = ntg_xy(0, 0);
    struct ntg_xy east_pos = ntg_xy(west_size.x + center_size.x, north_size.y); 
    struct ntg_xy south_pos = ntg_xy(0, north_size.y + center_size.y);
    struct ntg_xy west_pos = ntg_xy(0, north_size.y);
    struct ntg_xy center_pos = ntg_xy(west_size.x, north_size.y);

    struct ntg_arrange_result north_result = { .pos = north_pos };
    struct ntg_arrange_result east_result = { .pos = east_pos };
    struct ntg_arrange_result south_result = { .pos = south_pos };
    struct ntg_arrange_result west_result = { .pos = west_pos };
    struct ntg_arrange_result center_result = { .pos = center_pos };

    if(box->_north != NULL)
        ntg_arrange_output_set(out_positions, box->_north, north_result);
    if(box->_east != NULL)
        ntg_arrange_output_set(out_positions, box->_east, east_result);
    if(box->_south != NULL)
        ntg_arrange_output_set(out_positions, box->_south, south_result);
    if(box->_west != NULL)
        ntg_arrange_output_set(out_positions, box->_west, west_result);
    if(box->_center != NULL)
        ntg_arrange_output_set(out_positions, box->_center, center_result);
}

void ntg_border_box_set_north(ntg_border_box* box, ntg_object* north)
{
    assert(box != NULL);

    ntg_object* _box = NTG_OBJECT(box);

    if(box->_north != NULL)
        _ntg_object_rm_child(NTG_OBJECT(box), box->_north);

    if(north != NULL)
    {
        ntg_object* group_root = ntg_object_get_group_root(north);
        ntg_object* parent = ntg_object_get_parent(north,
                NTG_OBJECT_GET_PARENT_EXCL_DECORATOR);

        assert(parent == NULL);

        _ntg_object_add_child(_box, group_root);
    }

    box->_north = north;
}

void ntg_border_box_set_east(ntg_border_box* box, ntg_object* east)
{
    assert(box != NULL);

    ntg_object* _box = NTG_OBJECT(box);

    if(box->_east != NULL)
        _ntg_object_rm_child(NTG_OBJECT(box), box->_east);

    if(east != NULL)
    {
        ntg_object* group_root = ntg_object_get_group_root(east);
        ntg_object* parent = ntg_object_get_parent(east,
                NTG_OBJECT_GET_PARENT_EXCL_DECORATOR);

        assert(parent == NULL);

        _ntg_object_add_child(_box, group_root);
    }

    box->_east = east;
}

void ntg_border_box_set_south(ntg_border_box* box, ntg_object* south)
{
    assert(box != NULL);

    ntg_object* _box = NTG_OBJECT(box);

    if(box->_south != NULL)
        _ntg_object_rm_child(NTG_OBJECT(box), box->_south);

    if(south)
    {
        ntg_object* group_root = ntg_object_get_group_root(south);
        ntg_object* parent = ntg_object_get_parent(south,
                NTG_OBJECT_GET_PARENT_EXCL_DECORATOR);

        assert(parent == NULL);

        _ntg_object_add_child(_box, group_root);
    }

    box->_south = south;
}

void ntg_border_box_set_west(ntg_border_box* box, ntg_object* west)
{
    assert(box != NULL);

    ntg_object* _box = NTG_OBJECT(box);

    if(box->_west != NULL)
        _ntg_object_rm_child(NTG_OBJECT(box), box->_west);

    if(west)
    {
        ntg_object* group_root = ntg_object_get_group_root(west);
        ntg_object* parent = ntg_object_get_parent(west,
                NTG_OBJECT_GET_PARENT_EXCL_DECORATOR);

        assert(parent == NULL);

        _ntg_object_add_child(_box, group_root);
    }

    box->_west = west;
}

void ntg_border_box_set_center(ntg_border_box* box, ntg_object* center)
{
    assert(box != NULL);

    ntg_object* _box = NTG_OBJECT(box);

    if(box->_center != NULL)
        _ntg_object_rm_child(NTG_OBJECT(box), box->_center);

    if(center)
    {
        ntg_object* group_root = ntg_object_get_group_root(center);
        ntg_object* parent = ntg_object_get_parent(center,
                NTG_OBJECT_GET_PARENT_EXCL_DECORATOR);

        assert(parent == NULL);

        _ntg_object_add_child(_box, group_root);
    }

    box->_center = center;
}
