#include <assert.h>
#include <stdlib.h>

#include "core/object/ntg_border_box.h"
#include "core/object/shared/ntg_object_measure.h"
#include "core/object/shared/ntg_object_measure_map.h"
#include "core/object/shared/ntg_object_size_map.h"
#include "core/object/shared/ntg_object_xy_map.h"
#include "base/ntg_sap.h"
#include "shared/_ntg_shared.h"

static void get_children(const ntg_border_box* box, ntg_object** out_north,
        ntg_object** out_east, ntg_object** out_south,
        ntg_object** out_west, ntg_object** out_center)
{
    (*out_north) = box->_children[NTG_BORDER_BOX_NORTH];
    (*out_east) = box->_children[NTG_BORDER_BOX_EAST];
    (*out_south) = box->_children[NTG_BORDER_BOX_SOUTH];
    (*out_west) = box->_children[NTG_BORDER_BOX_WEST];
    (*out_center) = box->_children[NTG_BORDER_BOX_CENTER];
}

struct ntg_border_box_opts ntg_border_box_opts_def()
{
    return (struct ntg_border_box_opts) {0};
}

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

static void init_default(ntg_border_box* box)
{
    memset(box->_children, 0, sizeof(ntg_object*) * 5);
}

ntg_border_box* ntg_border_box_new(ntg_entity_system* system)
{
    struct ntg_entity_init_data entity_data = {
        .type = &NTG_ENTITY_BORDER_BOX,
        .deinit_fn = _ntg_border_box_deinit_fn,
        .system = system
    };

    ntg_border_box* new = (ntg_border_box*)ntg_entity_create(entity_data);
    assert(new != NULL);

    return new;
}

void _ntg_border_box_init_(ntg_border_box* box)
{
    assert(box != NULL);

    struct ntg_object_layout_ops object_data = {
        .init_fn = NULL,
        .deinit_fn = NULL,
        .measure_fn = _ntg_border_box_measure_fn,
        .constrain_fn = _ntg_border_box_constrain_fn,
        .arrange_fn = _ntg_border_box_arrange_fn,
        .draw_fn = NULL,
    };

    _ntg_object_init_((ntg_object*)box, object_data);

    init_default(box);
}

void ntg_border_box_set(ntg_border_box* box,
        ntg_object* object,
        ntg_border_box_pos position)
{
    assert(box != NULL);

    assert(box != NULL);
    assert((object == NULL) || ntg_object_is_widget(object));
    assert((ntg_object*)box != object);

    ntg_object* old_object = box->_children[position];

    if(old_object == object) return;

    if(object != NULL)
    {
        ntg_object* object_parent = ntg_object_get_parent_(object, false);
        assert(object_parent == NULL);
    }

    if(old_object != NULL)
        _ntg_object_rm_child((ntg_object*)box, old_object);

    box->_children[position] = object;

    if(object != NULL)
        _ntg_object_add_child((ntg_object*)box, object);

    _ntg_object_mark_change(object);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_border_box_deinit_fn(ntg_entity* entity)
{
    assert(entity != NULL);

    init_default((ntg_border_box*)entity);
    _ntg_object_deinit_fn(entity);
}

struct ntg_object_measure _ntg_border_box_measure_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        struct ntg_object_measure_ctx ctx,
        struct ntg_object_measure_out* out,
        void* layout_data,
        sarena* arena)
{
    // const ntg_object* object = (const ntg_object*)user;
    const ntg_border_box* box = (const ntg_border_box*)object;

    ntg_object *north, *east, *south, *west, *center;
    get_children(box, &north, &east, &south, &west, &center);

    struct ntg_object_measure north_measure = (north != NULL) ?
        ntg_object_measure_map_get(ctx.measures, north) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure east_measure = (east != NULL) ?
        ntg_object_measure_map_get(ctx.measures, east) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure south_measure = (south != NULL) ?
        ntg_object_measure_map_get(ctx.measures, south) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure west_measure = (west != NULL) ?
        ntg_object_measure_map_get(ctx.measures, west) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure center_measure = (center != NULL) ?
        ntg_object_measure_map_get(ctx.measures, center) :
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

void _ntg_border_box_constrain_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t size,
        struct ntg_object_constrain_ctx ctx,
        struct ntg_object_constrain_out* out,
        void* layout_data,
        sarena* arena)
{
    // const ntg_object* object = (const ntg_object*)user;
    const ntg_border_box* box = (const ntg_border_box*)object;

    ntg_object *north, *east, *south, *west, *center;
    get_children(box, &north, &east, &south, &west, &center);

    struct ntg_object_measure north_measure = (north != NULL) ?
        ntg_object_measure_map_get(ctx.measures, north) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure east_measure = (east != NULL) ?
        ntg_object_measure_map_get(ctx.measures, east) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure south_measure = (south != NULL) ?
        ntg_object_measure_map_get(ctx.measures, south) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure west_measure = (west != NULL) ?
        ntg_object_measure_map_get(ctx.measures, west) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure center_measure = (center != NULL) ?
        ntg_object_measure_map_get(ctx.measures, center) :
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

            ntg_sap_cap_round_robin(caps, grows, _sizes, extra_size, 3, arena);
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

            ntg_sap_cap_round_robin(caps, NULL, _sizes, extra_size, 3, arena);
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

            ntg_sap_cap_round_robin(caps, NULL, _sizes, extra_size, 3, arena);
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

            ntg_sap_cap_round_robin(caps, grows, _sizes, extra_size, 3, arena);

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

            ntg_sap_cap_round_robin(caps, NULL, _sizes, extra_size, 3, arena);
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

            ntg_sap_cap_round_robin(caps, NULL, _sizes, extra_size, 3, arena);
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

    if(north != NULL)
        ntg_object_size_map_set(out->sizes, north, north_size);
    if(east != NULL)
        ntg_object_size_map_set(out->sizes, east, east_size);
    if(south != NULL)
        ntg_object_size_map_set(out->sizes, south, south_size);
    if(west != NULL)
        ntg_object_size_map_set(out->sizes, west, west_size);
    if(center != NULL)
        ntg_object_size_map_set(out->sizes, center, center_size);
}

void _ntg_border_box_arrange_fn(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_arrange_ctx ctx,
        struct ntg_object_arrange_out* out,
        void* layout_data,
        sarena* arena)
{
    const ntg_border_box* box = (const ntg_border_box*)object;

    ntg_object *north, *east, *south, *west, *center;
    get_children(box, &north, &east, &south, &west, &center);

    struct ntg_xy north_size = (north != NULL) ?
        ntg_object_xy_map_get(ctx.sizes, north) : ntg_xy(0, 0);
    struct ntg_xy east_size = (east != NULL) ?
        ntg_object_xy_map_get(ctx.sizes, east) : ntg_xy(0, 0);
    struct ntg_xy south_size = (south != NULL) ?
        ntg_object_xy_map_get(ctx.sizes, south) : ntg_xy(0, 0);
    struct ntg_xy west_size = (west != NULL) ?
        ntg_object_xy_map_get(ctx.sizes, west) : ntg_xy(0, 0);

    size_t west_east_width = west_size.x + east_size.x;
    size_t north_south_height = north_size.y + south_size.y;
    struct ntg_xy collective_side_size = ntg_xy(west_east_width, north_south_height);

    struct ntg_xy center_size = ntg_xy_size(ntg_xy_sub(size, collective_side_size));

    struct ntg_xy north_pos = ntg_xy(0, 0);
    struct ntg_xy east_pos = ntg_xy(west_size.x + center_size.x, north_size.y); 
    struct ntg_xy south_pos = ntg_xy(0, north_size.y + center_size.y);
    struct ntg_xy west_pos = ntg_xy(0, north_size.y);
    struct ntg_xy center_pos = ntg_xy(west_size.x, north_size.y);

    if(north != NULL)
        ntg_object_xy_map_set(out->positions, north, north_pos);
    if(east != NULL)
        ntg_object_xy_map_set(out->positions, east, east_pos);
    if(south != NULL)
        ntg_object_xy_map_set(out->positions, south, south_pos);
    if(west != NULL)
        ntg_object_xy_map_set(out->positions, west, west_pos);
    if(center != NULL)
        ntg_object_xy_map_set(out->positions, center, center_pos);
}

