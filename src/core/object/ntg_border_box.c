#include <assert.h>

#include "core/object/ntg_border_box.h"
#include "core/object/shared/ntg_object_measure.h"
#include "core/object/shared/ntg_object_measures.h"
#include "core/object/shared/ntg_object_sizes.h"
#include "core/object/shared/ntg_object_types.h"
#include "core/object/shared/ntg_object_xys.h"
#include "base/ntg_sap.h"
#include "shared/_ntg_shared.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */

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
        ntg_object_process_key_fn process_key_fn,
        ntg_object_focus_fn on_focus_fn,
        ntg_object_unfocus_fn on_unfocus_fn,
        void* data)
{
    assert(box != NULL);

    __ntg_object_init__((ntg_object*)box,
            NTG_OBJECT_BORDER_BOX,
            __ntg_border_box_measure_fn,
            __ntg_border_box_constrain_fn,
            __ntg_border_box_arrange_fn,
            NULL,
            process_key_fn,
            on_focus_fn,
            on_unfocus_fn,
            __ntg_border_box_deinit_fn,
            data);

    __init_default(box);
}

void __ntg_border_box_deinit__(ntg_border_box* box)
{
    assert(box != NULL);

    __ntg_border_box_deinit_fn((ntg_object*)box);
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

    ntg_object* _box = (ntg_object*)box;

    ntg_object* parent = ntg_object_get_parent(north, NTG_OBJECT_PARENT_EXCL_DECOR);

    assert(parent == NULL);

    if(box->__north != NULL)
        _ntg_object_rm_child((ntg_object*)box, box->__north);

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

    ntg_object* _box = (ntg_object*)box;

    ntg_object* parent = ntg_object_get_parent(east, NTG_OBJECT_PARENT_EXCL_DECOR);

    assert(parent == NULL);

    if(box->__east != NULL)
        _ntg_object_rm_child((ntg_object*)box, box->__east);

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

    ntg_object* _box = (ntg_object*)box;

    ntg_object* parent = ntg_object_get_parent(south, NTG_OBJECT_PARENT_EXCL_DECOR);

    assert(parent == NULL);

    if(box->__south != NULL)
        _ntg_object_rm_child((ntg_object*)box, box->__south);

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

    ntg_object* _box = (ntg_object*)box;

    ntg_object* parent = ntg_object_get_parent(west, NTG_OBJECT_PARENT_EXCL_DECOR);

    assert(parent == NULL);

    if(box->__west != NULL)
        _ntg_object_rm_child((ntg_object*)box, box->__west);

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

    ntg_object* _box = (ntg_object*)box;

    ntg_object* parent = ntg_object_get_parent(center, NTG_OBJECT_PARENT_EXCL_DECOR);

    assert(parent == NULL);

    if(box->__center != NULL)
        _ntg_object_rm_child((ntg_object*)box, box->__center);

    if(center)
    {
        ntg_object* group_root = ntg_object_get_group_root(center);

        _ntg_object_add_child(_box, group_root);
    }

    box->__center = center;
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void __ntg_border_box_deinit_fn(ntg_object* _border_box)
{
    assert(_border_box != NULL);

    __ntg_object_deinit__(_border_box);
    __init_default((ntg_border_box*)_border_box);
}

struct ntg_object_measure __ntg_border_box_measure_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        struct ntg_object_measure_ctx ctx,
        struct ntg_object_measure_out* out,
        sarena* arena)
{
    // const ntg_object* object = (const ntg_object*)user;
    const ntg_border_box* box = (const ntg_border_box*)object;

    struct ntg_object_measure north_measure = (box->__north != NULL) ?
        ntg_object_measures_get(ctx.measures, box->__north) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure east_measure = (box->__east != NULL) ?
        ntg_object_measures_get(ctx.measures, box->__east) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure south_measure = (box->__south != NULL) ?
        ntg_object_measures_get(ctx.measures, box->__south) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure west_measure = (box->__west != NULL) ?
        ntg_object_measures_get(ctx.measures, box->__west) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure center_measure = (box->__center != NULL) ?
        ntg_object_measures_get(ctx.measures, box->__center) :
        (struct ntg_object_measure) {0};

    size_t min, natural, max;
    if(orientation == NTG_ORIENTATION_H)
    {
        min = _max3_size(
                north_measure.min_size,
                east_measure.min_size + center_measure.min_size + west_measure.min_size,
                south_measure.min_size);
        natural = _max3_size(
                north_measure.natural_size,
                east_measure.natural_size + center_measure.natural_size + west_measure.natural_size,
                south_measure.natural_size);
        max = _max3_size(
                north_measure.max_size,
                east_measure.max_size + center_measure.max_size + west_measure.max_size,
                south_measure.max_size);
    }
    else
    {
        min = _max3_size(
                north_measure.min_size + west_measure.min_size + south_measure.min_size,
                north_measure.min_size + center_measure.min_size + south_measure.min_size,
                north_measure.min_size + east_measure.min_size + south_measure.min_size);
        natural = _max3_size(
                north_measure.natural_size + west_measure.natural_size + south_measure.natural_size,
                north_measure.natural_size + center_measure.natural_size + south_measure.natural_size,
                north_measure.natural_size + east_measure.natural_size + south_measure.natural_size);
        max = _max3_size(
                north_measure.max_size + west_measure.max_size + south_measure.max_size,
                north_measure.max_size + center_measure.max_size + south_measure.max_size,
                north_measure.max_size + east_measure.max_size + south_measure.max_size);
    }

    return (struct ntg_object_measure) {
        .min_size = min,
        .natural_size = natural,
        .max_size = max,
        .grow = 1
    };
}

void __ntg_border_box_constrain_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t size,
        struct ntg_object_constrain_ctx ctx,
        struct ntg_object_constrain_out* out,
        sarena* arena)
{
    // const ntg_object* object = (const ntg_object*)user;
    const ntg_border_box* box = (const ntg_border_box*)object;

    struct ntg_object_measure north_measure = (box->__north != NULL) ?
        ntg_object_measures_get(ctx.measures, box->__north) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure east_measure = (box->__east != NULL) ?
        ntg_object_measures_get(ctx.measures, box->__east) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure south_measure = (box->__south != NULL) ?
        ntg_object_measures_get(ctx.measures, box->__south) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure west_measure = (box->__west != NULL) ?
        ntg_object_measures_get(ctx.measures, box->__west) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure center_measure = (box->__center != NULL) ?
        ntg_object_measures_get(ctx.measures, box->__center) :
        (struct ntg_object_measure) {0};

    // size_t min_size = ntg_constrain_ctx_get_min_size(ctx);
    // size_t natural_size = ntg_constrain_ctx_get_natural_size(ctx);
    // size_t max_size = ntg_constrain_ctx_get_max_size(ctx);

    size_t caps[3] = {0};
    size_t _sizes[3] = {0};
    size_t extra_size = 0;

    size_t north_size, east_size, south_size,
           west_size, center_size;

    if(orientation == NTG_ORIENTATION_H)
    {
        north_size = size;
        south_size = size;

        struct ntg_object_measure wce_measure = {
            .min_size = west_measure.min_size + center_measure.min_size + east_measure.min_size,
            .natural_size = west_measure.natural_size + center_measure.natural_size + east_measure.natural_size,
            .max_size = west_measure.max_size + center_measure.max_size + east_measure.max_size,
            .grow = 0 // ?
        };

        if(size >= wce_measure.natural_size)
        {
            caps[0] = west_measure.max_size;
            caps[1] = center_measure.max_size;
            caps[2] = east_measure.max_size;
            _sizes[0] = west_measure.natural_size;
            _sizes[1] = center_measure.natural_size;
            _sizes[2] = east_measure.natural_size;
            extra_size = size - wce_measure.natural_size;

            size_t grows[3];
            grows[0] = west_measure.grow;
            grows[1] = center_measure.grow;
            grows[2] = east_measure.grow;

            ntg_sap_cap_round_robin(caps, grows, _sizes, extra_size, 3);
        }
        else if(size > wce_measure.min_size)
        {
            caps[0] = west_measure.natural_size;
            caps[1] = center_measure.natural_size;
            caps[2] = east_measure.natural_size;
            _sizes[0] = west_measure.min_size;
            _sizes[1] = center_measure.min_size;
            _sizes[2] = east_measure.min_size;
            extra_size = size - wce_measure.min_size;

            ntg_sap_cap_round_robin(caps, NULL, _sizes, extra_size, 3);
        }
        else // size < wce_measure.min-size
        {
            caps[0] = west_measure.min_size;
            caps[1] = center_measure.min_size;
            caps[2] = east_measure.min_size;
            _sizes[0] = 0;
            _sizes[1] = 0;
            _sizes[2] = 0;
            extra_size = size;

            ntg_sap_cap_round_robin(caps, NULL, _sizes, extra_size, 3);
        }

        size_t alloced_size = _sizes[0] + _sizes[1] + _sizes[2];
        if(alloced_size < size)
            _sizes[1] = size - _sizes[0] - _sizes[2];

        west_size =  _sizes[0];
        center_size =  _sizes[1];
        east_size =  _sizes[2];
    }
    else // NTG_ORIENTATION_VERTICAL
    {
        struct ntg_object_measure wce_measure = {
            .min_size = _max3_size(west_measure.min_size, center_measure.min_size, east_measure.min_size),
            .natural_size = _max3_size(west_measure.natural_size, center_measure.natural_size, east_measure.natural_size),
            .max_size = _max3_size(west_measure.max_size, center_measure.max_size, east_measure.max_size),
            .grow = _max3_size(west_measure.grow, center_measure.grow, east_measure.grow)
        };

        struct ntg_object_measure total_measure = {
            .min_size = wce_measure.min_size + north_measure.min_size + south_measure.min_size,
            .natural_size = wce_measure.natural_size + north_measure.natural_size + south_measure.natural_size,
            .max_size = wce_measure.max_size + north_measure.max_size + south_measure.max_size,
            .grow = 0 // ?
        };

        if(size >= total_measure.natural_size)
        {
            caps[0] = north_measure.max_size;
            caps[1] = wce_measure.max_size;
            caps[2] = south_measure.max_size;
            _sizes[0] = north_measure.natural_size;
            _sizes[1] = wce_measure.natural_size;
            _sizes[2] = south_measure.natural_size;
            extra_size = size - total_measure.natural_size;

            size_t grows[3];
            grows[0] = north_measure.grow;
            grows[1] = wce_measure.grow;
            grows[2] = south_measure.grow;

            ntg_sap_cap_round_robin(caps, grows, _sizes, extra_size, 3);

        }
        else if(size >= total_measure.min_size)
        {
            caps[0] = north_measure.natural_size;
            caps[1] = wce_measure.natural_size;
            caps[2] = south_measure.natural_size;
            _sizes[0] = north_measure.min_size;
            _sizes[1] = wce_measure.min_size;
            _sizes[2] = south_measure.min_size;
            extra_size = size - total_measure.min_size;

            ntg_sap_cap_round_robin(caps, NULL, _sizes, extra_size, 3);
        }
        else
        {
            caps[0] = north_measure.min_size;
            caps[1] = wce_measure.min_size;
            caps[2] = south_measure.min_size;
            _sizes[0] = 0;
            _sizes[1] = 0;
            _sizes[2] = 0;
            extra_size = size;

            ntg_sap_cap_round_robin(caps, NULL, _sizes, extra_size, 3);
        }

        size_t alloced_size = _sizes[0] + _sizes[1] + _sizes[2];
        if(alloced_size < size) // add more
            _sizes[1] = size - _sizes[0] - _sizes[2];

        north_size =  _sizes[0];
        south_size =  _sizes[2];

        east_size =  _sizes[1];
        west_size =  _sizes[1];
        center_size =  _sizes[1];
    }

    if(box->__north != NULL)
        ntg_object_sizes_set(out->sizes, box->__north, north_size);
    if(box->__east != NULL)
        ntg_object_sizes_set(out->sizes, box->__east, east_size);
    if(box->__south != NULL)
        ntg_object_sizes_set(out->sizes, box->__south, south_size);
    if(box->__west != NULL)
        ntg_object_sizes_set(out->sizes, box->__west, west_size);
    if(box->__center != NULL)
        ntg_object_sizes_set(out->sizes, box->__center, center_size);
}

void __ntg_border_box_arrange_fn(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_arrange_ctx ctx,
        struct ntg_object_arrange_out* out,
        sarena* arena)
{
    const ntg_border_box* box = (const ntg_border_box*)object;

    struct ntg_xy north_size = (box->__north != NULL) ?
        ntg_object_xys_get(ctx.sizes, box->__north) : ntg_xy(0, 0);
    struct ntg_xy east_size = (box->__east != NULL) ?
        ntg_object_xys_get(ctx.sizes, box->__east) : ntg_xy(0, 0);
    struct ntg_xy south_size = (box->__south != NULL) ?
        ntg_object_xys_get(ctx.sizes, box->__south) : ntg_xy(0, 0);
    struct ntg_xy west_size = (box->__west != NULL) ?
        ntg_object_xys_get(ctx.sizes, box->__west) : ntg_xy(0, 0);

    size_t west_east_width = west_size.x + east_size.x;
    size_t north_south_height = north_size.y + south_size.y;
    struct ntg_xy collective_side_size = ntg_xy(west_east_width, north_south_height);

    struct ntg_xy center_size = ntg_xy_size(ntg_xy_sub(size, collective_side_size));

    struct ntg_xy north_pos = ntg_xy(0, 0);
    struct ntg_xy east_pos = ntg_xy(west_size.x + center_size.x, north_size.y); 
    struct ntg_xy south_pos = ntg_xy(0, north_size.y + center_size.y);
    struct ntg_xy west_pos = ntg_xy(0, north_size.y);
    struct ntg_xy center_pos = ntg_xy(west_size.x, north_size.y);

    if(box->__north != NULL)
        ntg_object_xys_set(out->pos, box->__north, north_pos);
    if(box->__east != NULL)
        ntg_object_xys_set(out->pos, box->__east, east_pos);
    if(box->__south != NULL)
        ntg_object_xys_set(out->pos, box->__south, south_pos);
    if(box->__west != NULL)
        ntg_object_xys_set(out->pos, box->__west, west_pos);
    if(box->__center != NULL)
        ntg_object_xys_set(out->pos, box->__center, center_pos);
}

