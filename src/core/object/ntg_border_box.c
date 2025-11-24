#include <assert.h>

#include "core/object/ntg_border_box.h"
#include "core/object/shared/ntg_object_types.h"
#include "core/scene/shared/ntg_drawable_kit.h"
#include "base/ntg_sap.h"
#include "shared/_ntg_shared.h"

static void __init_default(ntg_border_box* box)
{
    box->__north = NULL;
    box->__east = NULL;
    box->__south = NULL;
    box->__west = NULL;
    box->__center = NULL;
}

void __ntg_border_box_init__(
        ntg_border_box* box,
        ntg_process_key_fn process_key_fn,
        ntg_on_focus_fn on_focus_fn,
        void* data)
{
    assert(box != NULL);

    __ntg_object_init__(NTG_OBJECT(box),
            NTG_OBJECT_BORDER_BOX,
            __ntg_border_box_measure_fn,
            __ntg_border_box_constrain_fn,
            __ntg_border_box_arrange_fn,
            NULL,
            process_key_fn,
            on_focus_fn,
            NULL,
            NULL,
            data);

    __init_default(box);
}

void __ntg_border_box_deinit__(ntg_border_box* box)
{
    assert(box != NULL);

    __init_default(box);
}

struct ntg_measure_output __ntg_border_box_measure_fn(
        const ntg_drawable* drawable,
        ntg_orientation orientation,
        size_t for_size,
        const ntg_measure_context* context)
{
    // const ntg_object* object = (const ntg_object*)user;
    const ntg_border_box* box = ntg_drawable_user(drawable);

    const ntg_drawable* north_drawable = (box->__north != NULL) ?
        ntg_object_get_drawable(box->__north) :
        NULL;
    const ntg_drawable* east_drawable = (box->__east != NULL) ?
        ntg_object_get_drawable(box->__east) :
        NULL;
    const ntg_drawable* south_drawable = (box->__south != NULL) ?
        ntg_object_get_drawable(box->__south) :
        NULL;
    const ntg_drawable* west_drawable = (box->__west != NULL) ?
        ntg_object_get_drawable(box->__west) :
        NULL;
    const ntg_drawable* center_drawable = (box->__center != NULL) ?
        ntg_object_get_drawable(box->__center) :
        NULL;

    struct ntg_measure_data north_data = (north_drawable != NULL) ?
        ntg_measure_context_get(context, north_drawable) :
        (struct ntg_measure_data) {0};
    struct ntg_measure_data east_data = (east_drawable != NULL) ?
        ntg_measure_context_get(context, east_drawable) :
        (struct ntg_measure_data) {0};
    struct ntg_measure_data south_data = (south_drawable != NULL) ?
        ntg_measure_context_get(context, south_drawable) :
        (struct ntg_measure_data) {0};
    struct ntg_measure_data west_data = (west_drawable != NULL) ?
        ntg_measure_context_get(context, west_drawable) :
        (struct ntg_measure_data) {0};
    struct ntg_measure_data center_data = (center_drawable != NULL) ?
        ntg_measure_context_get(context, center_drawable) :
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

    return (struct ntg_measure_output) {
        .min_size = min,
        .natural_size = natural,
        .max_size = max
    };
}

void __ntg_border_box_constrain_fn(
        const ntg_drawable* drawable,
        ntg_orientation orientation,
        size_t size,
        struct ntg_measure_output measure_output,
        const ntg_constrain_context* context,
        ntg_constrain_output* out_sizes)
{
    // const ntg_object* object = (const ntg_object*)user;
    const ntg_border_box* box = ntg_drawable_user(drawable);

    const ntg_drawable* north_drawable = (box->__north != NULL) ?
        ntg_object_get_drawable(box->__north) :
        NULL;
    const ntg_drawable* east_drawable = (box->__east != NULL) ?
        ntg_object_get_drawable(box->__east) :
        NULL;
    const ntg_drawable* south_drawable = (box->__south != NULL) ?
        ntg_object_get_drawable(box->__south) :
        NULL;
    const ntg_drawable* west_drawable = (box->__west != NULL) ?
        ntg_object_get_drawable(box->__west) :
        NULL;
    const ntg_drawable* center_drawable = (box->__center != NULL) ?
        ntg_object_get_drawable(box->__center) :
        NULL;

    struct ntg_constrain_data north_data = (north_drawable != NULL) ?
        ntg_constrain_context_get(context, north_drawable) :
        (struct ntg_constrain_data) {0};
    struct ntg_constrain_data east_data = (east_drawable != NULL) ?
        ntg_constrain_context_get(context, east_drawable) :
        (struct ntg_constrain_data) {0};
    struct ntg_constrain_data south_data = (south_drawable != NULL) ?
        ntg_constrain_context_get(context, south_drawable) :
        (struct ntg_constrain_data) {0};
    struct ntg_constrain_data west_data = (west_drawable != NULL) ?
        ntg_constrain_context_get(context, west_drawable) :
        (struct ntg_constrain_data) {0};
    struct ntg_constrain_data center_data = (center_drawable != NULL) ?
        ntg_constrain_context_get(context, center_drawable) :
        (struct ntg_constrain_data) {0};

    // size_t min_size = ntg_constrain_context_get_min_size(context);
    // size_t natural_size = ntg_constrain_context_get_natural_size(context);
    // size_t max_size = ntg_constrain_context_get_max_size(context);

    size_t caps[3] = {0};
    size_t _sizes[3] = {0};
    size_t extra_size = 0;

    struct ntg_constrain_result north_result, east_result, south_result,
                                west_result, center_result;

    if(orientation == NTG_ORIENTATION_HORIZONTAL)
    {
        north_result = (struct ntg_constrain_result) { .size = size };
        south_result = (struct ntg_constrain_result) { .size = size };

        struct ntg_constrain_data wce_data = {
            .min_size = west_data.min_size + center_data.min_size + east_data.min_size,
            .natural_size = west_data.natural_size + center_data.natural_size + east_data.natural_size,
            .max_size = west_data.max_size + center_data.max_size + east_data.max_size,
            .grow = 0 // ?
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

            size_t grows[3];
            grows[0] = west_data.grow;
            grows[1] = center_data.grow;
            grows[2] = east_data.grow;

            ntg_sap_cap_round_robin(caps, grows, _sizes, extra_size, 3);
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

            ntg_sap_cap_round_robin(caps, NULL, _sizes, extra_size, 3);
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

            ntg_sap_cap_round_robin(caps, NULL, _sizes, extra_size, 3);
        }

        size_t alloced_size = _sizes[0] + _sizes[1] + _sizes[2];
        if(alloced_size < size)
            _sizes[1] = size - _sizes[0] - _sizes[2];

        west_result = (struct ntg_constrain_result) { .size = _sizes[0] };
        center_result = (struct ntg_constrain_result) { .size = _sizes[1] };
        east_result = (struct ntg_constrain_result) { .size = _sizes[2] };
    }
    else // NTG_ORIENTATION_VERTICAL
    {
        struct ntg_constrain_data wce_data = {
            .min_size = _max3_size(west_data.min_size, center_data.min_size, east_data.min_size),
            .natural_size = _max3_size(west_data.natural_size, center_data.natural_size, east_data.natural_size),
            .max_size = _max3_size(west_data.max_size, center_data.max_size, east_data.max_size),
            .grow = _max3_size(west_data.grow, center_data.grow, east_data.grow)
        };

        struct ntg_constrain_data total_data = {
            .min_size = wce_data.min_size + north_data.min_size + south_data.min_size,
            .natural_size = wce_data.natural_size + north_data.natural_size + south_data.natural_size,
            .max_size = wce_data.max_size + north_data.max_size + south_data.max_size,
            .grow = 0 // ?
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

            size_t grows[3];
            grows[0] = north_data.grow;
            grows[1] = wce_data.grow;
            grows[2] = south_data.grow;

            ntg_sap_cap_round_robin(caps, grows, _sizes, extra_size, 3);

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

            ntg_sap_cap_round_robin(caps, NULL, _sizes, extra_size, 3);
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

            ntg_sap_cap_round_robin(caps, NULL, _sizes, extra_size, 3);
        }

        size_t alloced_size = _sizes[0] + _sizes[1] + _sizes[2];
        if(alloced_size < size) // add more
            _sizes[1] = size - _sizes[0] - _sizes[2];

        north_result = (struct ntg_constrain_result) { .size = _sizes[0] };
        south_result = (struct ntg_constrain_result) { .size = _sizes[2] };

        east_result = (struct ntg_constrain_result) { .size = _sizes[1] };
        west_result = (struct ntg_constrain_result) { .size = _sizes[1] };
        center_result = (struct ntg_constrain_result) { .size = _sizes[1] };
    }

    if(box->__north != NULL)
        ntg_constrain_output_set(out_sizes, north_drawable, north_result);
    if(box->__east != NULL)
        ntg_constrain_output_set(out_sizes, east_drawable, east_result);
    if(box->__south != NULL)
        ntg_constrain_output_set(out_sizes, south_drawable, south_result);
    if(box->__west != NULL)
        ntg_constrain_output_set(out_sizes, west_drawable, west_result);
    if(box->__center != NULL)
        ntg_constrain_output_set(out_sizes, center_drawable, center_result);
}

void __ntg_border_box_arrange_fn(
        const ntg_drawable* drawable,
        struct ntg_xy size,
        const ntg_arrange_context* context,
        ntg_arrange_output* out_positions)
{
    // const ntg_object* object = (const ntg_object*)user;
    const ntg_border_box* box = ntg_drawable_user(drawable);

    const ntg_drawable* north_drawable = (box->__north != NULL) ?
        ntg_object_get_drawable(box->__north) :
        NULL;
    const ntg_drawable* east_drawable = (box->__east != NULL) ?
        ntg_object_get_drawable(box->__east) :
        NULL;
    const ntg_drawable* south_drawable = (box->__south != NULL) ?
        ntg_object_get_drawable(box->__south) :
        NULL;
    const ntg_drawable* west_drawable = (box->__west != NULL) ?
        ntg_object_get_drawable(box->__west) :
        NULL;
    const ntg_drawable* center_drawable = (box->__center != NULL) ?
        ntg_object_get_drawable(box->__center) :
        NULL;

    struct ntg_arrange_data north_data = (north_drawable != NULL) ?
        ntg_arrange_context_get(context, north_drawable) :
        (struct ntg_arrange_data) {0};
    struct ntg_arrange_data east_data = (east_drawable != NULL) ?
        ntg_arrange_context_get(context, east_drawable) :
        (struct ntg_arrange_data) {0};
    struct ntg_arrange_data south_data = (south_drawable != NULL) ?
        ntg_arrange_context_get(context, south_drawable) :
        (struct ntg_arrange_data) {0};
    struct ntg_arrange_data west_data = (west_drawable != NULL) ?
        ntg_arrange_context_get(context, west_drawable) :
        (struct ntg_arrange_data) {0};
    // struct ntg_arrange_data center_data = (center_drawable != NULL) ?
    //     ntg_arrange_context_get(context, center_drawable) :
    //     (struct ntg_arrange_data) {0};

    struct ntg_xy north_size = ntg_xy_size(north_data.size);
    struct ntg_xy east_size = ntg_xy_size(east_data.size);
    struct ntg_xy south_size = ntg_xy_size(south_data.size);
    struct ntg_xy west_size = ntg_xy_size(west_data.size);

    size_t west_east_width = west_size.x + east_size.x;
    size_t north_south_height = north_size.y + south_size.y;
    struct ntg_xy collective_side_size = ntg_xy(west_east_width, north_south_height);

    struct ntg_xy center_size = ntg_xy_size(ntg_xy_sub(size, collective_side_size));

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

    if(box->__north != NULL)
        ntg_arrange_output_set(out_positions, north_drawable, north_result);
    if(box->__east != NULL)
        ntg_arrange_output_set(out_positions, east_drawable, east_result);
    if(box->__south != NULL)
        ntg_arrange_output_set(out_positions, south_drawable, south_result);
    if(box->__west != NULL)
        ntg_arrange_output_set(out_positions, west_drawable, west_result);
    if(box->__center != NULL)
        ntg_arrange_output_set(out_positions, center_drawable, center_result);
}

ntg_object* ntg_border_box_get_north(ntg_border_box* box)
{
    assert(box != NULL);

    return box->__north;
}

ntg_object* ntg_border_box_get_east(ntg_border_box* box)
{
    assert(box != NULL);

    return box->__east;
}

ntg_object* ntg_border_box_get_south(ntg_border_box* box)
{
    assert(box != NULL);

    return box->__south;
}

ntg_object* ntg_border_box_get_west(ntg_border_box* box)
{
    assert(box != NULL);

    return box->__west;
}

ntg_object* ntg_border_box_get_center(ntg_border_box* box)
{
    assert(box != NULL);

    return box->__center;
}

void ntg_border_box_set_north(ntg_border_box* box, ntg_object* north)
{
    assert(box != NULL);

    ntg_object* _box = NTG_OBJECT(box);

    ntg_object* parent = ntg_object_get_parent(north,
            NTG_OBJECT_GET_PARENT_EXCL_DECORATOR);

    assert(parent == NULL);

    if(box->__north != NULL)
        _ntg_object_rm_child(NTG_OBJECT(box), box->__north);

    if(north != NULL)
    {
        ntg_object* group_root = ntg_object_get_group_root(north);

        _ntg_object_add_child(_box, group_root);
    }

    box->__north = north;
}

void ntg_border_box_set_east(ntg_border_box* box, ntg_object* east)
{
    assert(box != NULL);

    ntg_object* _box = NTG_OBJECT(box);

    ntg_object* parent = ntg_object_get_parent(east,
            NTG_OBJECT_GET_PARENT_EXCL_DECORATOR);

    assert(parent == NULL);

    if(box->__east != NULL)
        _ntg_object_rm_child(NTG_OBJECT(box), box->__east);

    if(east != NULL)
    {
        ntg_object* group_root = ntg_object_get_group_root(east);

        _ntg_object_add_child(_box, group_root);
    }

    box->__east = east;
}

void ntg_border_box_set_south(ntg_border_box* box, ntg_object* south)
{
    assert(box != NULL);

    ntg_object* _box = NTG_OBJECT(box);

    ntg_object* parent = ntg_object_get_parent(south,
            NTG_OBJECT_GET_PARENT_EXCL_DECORATOR);

    assert(parent == NULL);

    if(box->__south != NULL)
        _ntg_object_rm_child(NTG_OBJECT(box), box->__south);

    if(south)
    {
        ntg_object* group_root = ntg_object_get_group_root(south);

        _ntg_object_add_child(_box, group_root);
    }

    box->__south = south;
}

void ntg_border_box_set_west(ntg_border_box* box, ntg_object* west)
{
    assert(box != NULL);

    ntg_object* _box = NTG_OBJECT(box);

    ntg_object* parent = ntg_object_get_parent(west,
            NTG_OBJECT_GET_PARENT_EXCL_DECORATOR);

    assert(parent == NULL);

    if(box->__west != NULL)
        _ntg_object_rm_child(NTG_OBJECT(box), box->__west);

    if(west)
    {
        ntg_object* group_root = ntg_object_get_group_root(west);

        _ntg_object_add_child(_box, group_root);
    }

    box->__west = west;
}

void ntg_border_box_set_center(ntg_border_box* box, ntg_object* center)
{
    assert(box != NULL);

    ntg_object* _box = NTG_OBJECT(box);

    ntg_object* parent = ntg_object_get_parent(center,
            NTG_OBJECT_GET_PARENT_EXCL_DECORATOR);

    assert(parent == NULL);

    if(box->__center != NULL)
        _ntg_object_rm_child(NTG_OBJECT(box), box->__center);

    if(center)
    {
        ntg_object* group_root = ntg_object_get_group_root(center);

        _ntg_object_add_child(_box, group_root);
    }

    box->__center = center;
}
