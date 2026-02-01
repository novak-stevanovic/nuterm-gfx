#include "ntg.h"
#include <assert.h>
#include "shared/ntg_shared_internal.h"

static struct ntg_object_measure measure_fn(
        const ntg_object* _box,
        ntg_orient orient,
        void* lctx,
        sarena* arena);

static void constrain_fn(
        const ntg_object* _box,
        ntg_orient orient,
        ntg_object_size_map* out_size_map,
        void* lctx,
        sarena* arena);

static void arrange_fn(
        const ntg_object* _box,
        ntg_object_pos_map* out_pos_map,
        void* lctx,
        sarena* arena);

static void on_child_rm_fn(ntg_object* _bbox, ntg_object* child);

static void get_children(const ntg_bbox* box, ntg_object** out_north,
        ntg_object** out_east, ntg_object** out_south, ntg_object** out_west,
        ntg_object** out_center);

struct ntg_bbox_opts ntg_bbox_opts_def()
{
    return (struct ntg_bbox_opts) {0};
}

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

static void init_default(ntg_bbox* box)
{
    memset(box->_children, 0, sizeof(ntg_object*) * 5);
}

void ntg_bbox_init(ntg_bbox* box)
{
    assert(box != NULL);

    struct ntg_object_layout_ops layout_ops = {
        .measure_fn = measure_fn,
        .constrain_fn = constrain_fn,
        .arrange_fn = arrange_fn,
        .draw_fn = NULL,
        .init_fn = NULL,
        .deinit_fn = NULL
    };

    struct ntg_object_hooks hooks = {
        .on_child_rm_fn = on_child_rm_fn
    };

    ntg_object_init((ntg_object*)box, layout_ops, hooks, &NTG_TYPE_BBOX);

    init_default(box);
}

void ntg_bbox_set(
        ntg_bbox* box,
        ntg_object* object,
        enum ntg_bbox_pos pos)
{
    assert(box != NULL);

    if(box->_children[pos] == object) return;

    if(box->_children[pos] != NULL)
        ntg_object_detach(box->_children[pos]);

    if(object != NULL)
    {
        ntg_object_attach((ntg_object*)box, object);
        box->_children[pos] = object;
    }

    ntg_object_add_dirty((ntg_object*)box, NTG_OBJECT_DIRTY_FULL);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_bbox_deinit(ntg_bbox* box)
{
    init_default(box);
    ntg_object_deinit((ntg_object*)box);
}

static struct ntg_object_measure measure_fn(
        const ntg_object* _box,
        ntg_orient orient,
        void* lctx,
        sarena* arena)
{
    const ntg_bbox* bbox = (const ntg_bbox*)_box;

    ntg_object *north, *east, *south, *west, *center;
    get_children(bbox, &north, &east, &south, &west, &center);

    struct ntg_object_measure north_msr = (north != NULL) ?
        ntg_object_get_measure(north, orient) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure east_msr = (east != NULL) ?
        ntg_object_get_measure(east, orient) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure south_msr = (south != NULL) ?
        ntg_object_get_measure(south, orient) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure west_msr = (west != NULL) ?
        ntg_object_get_measure(west, orient) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure center_msr = (center != NULL) ?
        ntg_object_get_measure(center, orient) :
        (struct ntg_object_measure) {0};

    size_t min, nat, max;
    if(orient == NTG_ORIENT_H)
    {
        min = _max3_size(
                north_msr.min_size,
                east_msr.min_size + center_msr.min_size + west_msr.min_size,
                south_msr.min_size);
        nat = _max3_size(
                north_msr.nat_size,
                east_msr.nat_size + center_msr.nat_size + west_msr.nat_size,
                south_msr.nat_size);
        max = _max3_size(
                north_msr.max_size,
                east_msr.max_size + center_msr.max_size + west_msr.max_size,
                south_msr.max_size);
    }
    else
    {
        min = _max3_size(
                north_msr.min_size + west_msr.min_size + south_msr.min_size,
                north_msr.min_size + center_msr.min_size + south_msr.min_size,
                north_msr.min_size + east_msr.min_size + south_msr.min_size);
        nat = _max3_size(
                north_msr.nat_size + west_msr.nat_size + south_msr.nat_size,
                north_msr.nat_size + center_msr.nat_size + south_msr.nat_size,
                north_msr.nat_size + east_msr.nat_size + south_msr.nat_size);
        max = _max3_size(
                north_msr.max_size + west_msr.max_size + south_msr.max_size,
                north_msr.max_size + center_msr.max_size + south_msr.max_size,
                north_msr.max_size + east_msr.max_size + south_msr.max_size);
    }

    return (struct ntg_object_measure) {
        .min_size = min,
        .nat_size = nat,
        .max_size = max,
        .grow = 1
    };
}

static void constrain_fn(
        const ntg_object* _box,
        ntg_orient orient,
        ntg_object_size_map* out_size_map,
        void* lctx,
        sarena* arena)
{
    const ntg_bbox* bbox = (const ntg_bbox*)_box;
    size_t size = ntg_object_get_size_1d_cont(_box, orient);

    ntg_object *north, *east, *south, *west, *center;
    get_children(bbox, &north, &east, &south, &west, &center);

    struct ntg_object_measure north_msr = (north != NULL) ?
        ntg_object_get_measure(north, orient) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure east_msr = (east != NULL) ?
        ntg_object_get_measure(east, orient) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure south_msr = (south != NULL) ?
        ntg_object_get_measure(south, orient) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure west_msr = (west != NULL) ?
        ntg_object_get_measure(west, orient) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure center_msr = (center != NULL) ?
        ntg_object_get_measure(center, orient) :
        (struct ntg_object_measure) {0};

    size_t caps[3] = {0};
    size_t _sizes[3] = {0};
    size_t extra_size = 0;

    size_t north_size, east_size, south_size, west_size, center_size;

    if(orient == NTG_ORIENT_H)
    {
        north_size = size;
        south_size = size;

        struct ntg_object_measure wce_msr = {
            .min_size = west_msr.min_size + center_msr.min_size + east_msr.min_size,
            .nat_size = west_msr.nat_size + center_msr.nat_size + east_msr.nat_size,
            .max_size = west_msr.max_size + center_msr.max_size + east_msr.max_size,
            .grow = 0 // ?
        };

        if(size >= wce_msr.nat_size)
        {
            caps[0] = west_msr.max_size;
            caps[1] = center_msr.max_size;
            caps[2] = east_msr.max_size;
            _sizes[0] = west_msr.nat_size;
            _sizes[1] = center_msr.nat_size;
            _sizes[2] = east_msr.nat_size;
            extra_size = size - wce_msr.nat_size;

            size_t grows[3];
            grows[0] = west_msr.grow;
            grows[1] = center_msr.grow;
            grows[2] = east_msr.grow;

            ntg_sap_cap_round_robin(caps, grows, _sizes, extra_size, 3, arena);
        }
        else if(size > wce_msr.min_size)
        {
            caps[0] = west_msr.nat_size;
            caps[1] = center_msr.nat_size;
            caps[2] = east_msr.nat_size;
            _sizes[0] = west_msr.min_size;
            _sizes[1] = center_msr.min_size;
            _sizes[2] = east_msr.min_size;
            extra_size = size - wce_msr.min_size;

            ntg_sap_cap_round_robin(caps, NULL, _sizes, extra_size, 3, arena);
        }
        else // size < wce_msr.min-size
        {
            caps[0] = west_msr.min_size;
            caps[1] = center_msr.min_size;
            caps[2] = east_msr.min_size;
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
    else // NTG_ORIENT_V
    {
        struct ntg_object_measure wce_msr = {
            .min_size = _max3_size(west_msr.min_size, center_msr.min_size, east_msr.min_size),
            .nat_size = _max3_size(west_msr.nat_size, center_msr.nat_size, east_msr.nat_size),
            .max_size = _max3_size(west_msr.max_size, center_msr.max_size, east_msr.max_size),
            .grow = _max3_size(west_msr.grow, center_msr.grow, east_msr.grow)
        };

        struct ntg_object_measure total_msr = {
            .min_size = wce_msr.min_size + north_msr.min_size + south_msr.min_size,
            .nat_size = wce_msr.nat_size + north_msr.nat_size + south_msr.nat_size,
            .max_size = wce_msr.max_size + north_msr.max_size + south_msr.max_size,
            .grow = 0 // ?
        };

        if(size >= total_msr.nat_size)
        {
            caps[0] = north_msr.max_size;
            caps[1] = wce_msr.max_size;
            caps[2] = south_msr.max_size;
            _sizes[0] = north_msr.nat_size;
            _sizes[1] = wce_msr.nat_size;
            _sizes[2] = south_msr.nat_size;
            extra_size = size - total_msr.nat_size;

            size_t grows[3];
            grows[0] = north_msr.grow;
            grows[1] = wce_msr.grow;
            grows[2] = south_msr.grow;

            ntg_sap_cap_round_robin(caps, grows, _sizes, extra_size, 3, arena);

        }
        else if(size >= total_msr.min_size)
        {
            caps[0] = north_msr.nat_size;
            caps[1] = wce_msr.nat_size;
            caps[2] = south_msr.nat_size;
            _sizes[0] = north_msr.min_size;
            _sizes[1] = wce_msr.min_size;
            _sizes[2] = south_msr.min_size;
            extra_size = size - total_msr.min_size;

            ntg_sap_cap_round_robin(caps, NULL, _sizes, extra_size, 3, arena);
        }
        else
        {
            caps[0] = north_msr.min_size;
            caps[1] = wce_msr.min_size;
            caps[2] = south_msr.min_size;
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
        ntg_object_size_map_set(out_size_map, north, north_size);
    if(east != NULL)
        ntg_object_size_map_set(out_size_map, east, east_size);
    if(south != NULL)
        ntg_object_size_map_set(out_size_map, south, south_size);
    if(west != NULL)
        ntg_object_size_map_set(out_size_map, west, west_size);
    if(center != NULL)
        ntg_object_size_map_set(out_size_map, center, center_size);
}

static void arrange_fn(
        const ntg_object* _box,
        ntg_object_pos_map* out_pos_map,
        void* lctx,
        sarena* arena)
{
    const ntg_bbox* bbox = (const ntg_bbox*)_box;
    struct ntg_xy size = ntg_object_get_size_cont(_box);

    ntg_object *north, *east, *south, *west, *center;
    get_children(bbox, &north, &east, &south, &west, &center);

    struct ntg_xy north_size = (north != NULL) ?
        north->_size : ntg_xy(0, 0);
    struct ntg_xy east_size = (east != NULL) ?
        east->_size : ntg_xy(0, 0);
    struct ntg_xy south_size = (south != NULL) ?
        south->_size : ntg_xy(0, 0);
    struct ntg_xy west_size = (west != NULL) ?
        west->_size : ntg_xy(0, 0);

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
        ntg_object_pos_map_set(out_pos_map, north, north_pos);
    if(east != NULL)
        ntg_object_pos_map_set(out_pos_map, east, east_pos);
    if(south != NULL)
        ntg_object_pos_map_set(out_pos_map, south, south_pos);
    if(west != NULL)
        ntg_object_pos_map_set(out_pos_map, west, west_pos);
    if(center != NULL)
        ntg_object_pos_map_set(out_pos_map, center, center_pos);
}

static void get_children(const ntg_bbox* box, ntg_object** out_north,
        ntg_object** out_east, ntg_object** out_south, ntg_object** out_west,
        ntg_object** out_center)
{
    (*out_north) = box->_children[NTG_BBOX_NORTH];
    (*out_east) = box->_children[NTG_BBOX_EAST];
    (*out_south) = box->_children[NTG_BBOX_SOUTH];
    (*out_west) = box->_children[NTG_BBOX_WEST];
    (*out_center) = box->_children[NTG_BBOX_CENTER];
}

static void on_child_rm_fn(ntg_object* _bbox, ntg_object* child)
{
    ntg_bbox* bbox = (ntg_bbox*)_bbox;

    size_t i;
    for(i = 0; i < 5; i++)
    {
        if(bbox->_children[i] == child)
            bbox->_children[i] = NULL;
    }
}
