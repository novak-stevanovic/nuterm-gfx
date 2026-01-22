#include "ntg.h"
#include "shared/_ntg_shared.h"
#include <assert.h>

static void rm_child_fn(ntg_widget* _border_box, ntg_widget* child);

static void 
get_children(const ntg_border_box* box, ntg_widget** out_north,
             ntg_widget** out_east, ntg_widget** out_south,
             ntg_widget** out_west, ntg_widget** out_center);

struct ntg_border_box_opts ntg_border_box_opts_def()
{
    return (struct ntg_border_box_opts) {0};
}

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

static void init_default(ntg_border_box* box)
{
    memset(box->_children, 0, sizeof(ntg_widget*) * 5);
}

ntg_border_box* ntg_border_box_new(ntg_entity_system* system)
{
    struct ntg_entity_init_data entity_data = {
        .type = &NTG_ENTITY_BORDER_BOX,
        .deinit_fn = ntg_border_box_deinit_fn,
        .system = system
    };

    ntg_border_box* new = (ntg_border_box*)ntg_entity_create(entity_data);
    assert(new != NULL);

    return new;
}

void ntg_border_box_init(ntg_border_box* box)
{
    assert(box != NULL);

    struct ntg_widget_layout_ops layout_ops = {
        .measure_fn = _ntg_border_box_measure_fn,
        .constrain_fn = _ntg_border_box_constrain_fn,
        .arrange_fn = _ntg_border_box_arrange_fn,
        .draw_fn = NULL,
        .init_fn = NULL,
        .deinit_fn = NULL
    };

    struct ntg_widget_hooks hooks = {
        .rm_child_fn = rm_child_fn
    };

    ntg_widget_init((ntg_widget*)box, layout_ops, hooks);

    init_default(box);
}

void ntg_border_box_set(ntg_border_box* box,
        ntg_widget* widget,
        enum ntg_border_box_pos pos)
{
    assert(box != NULL);

    if(box->_children[pos] == widget) return;

    if(box->_children[pos] != NULL)
        ntg_widget_detach(box->_children[pos]);

    if(widget != NULL)
    {
        ntg_widget_attach((ntg_widget*)box, widget);
        box->_children[pos] = widget;
    }

    ntg_entity_raise_event_((ntg_entity*)box, NTG_EVENT_OBJECT_DIFF, NULL);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_border_box_deinit_fn(ntg_entity* entity)
{
    assert(entity != NULL);

    init_default((ntg_border_box*)entity);
    ntg_widget_deinit_fn(entity);
}

struct ntg_object_measure _ntg_border_box_measure_fn(
        const ntg_widget* _border_box,
        void* _layout_data,
        ntg_orient orient,
        bool constrained,
        sarena* arena)
{
    const ntg_border_box* border_box = (const ntg_border_box*)_border_box;

    ntg_widget *north, *east, *south, *west, *center;
    get_children(border_box, &north, &east, &south, &west, &center);

    struct ntg_object_measure north_msr = (north != NULL) ?
        ntg_widget_get_measure(north, orient) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure east_msr = (east != NULL) ?
        ntg_widget_get_measure(east, orient) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure south_msr = (south != NULL) ?
        ntg_widget_get_measure(south, orient) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure west_msr = (west != NULL) ?
        ntg_widget_get_measure(west, orient) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure center_msr = (center != NULL) ?
        ntg_widget_get_measure(center, orient) :
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

void _ntg_border_box_constrain_fn(
        const ntg_widget* _border_box,
        void* _layout_data,
        ntg_orient orient,
        ntg_widget_size_map* out_size_map,
        sarena* arena)
{
    const ntg_border_box* border_box = (const ntg_border_box*)_border_box;
    size_t size = ntg_widget_get_cont_size_1d(_border_box, orient);

    ntg_widget *north, *east, *south, *west, *center;
    get_children(border_box, &north, &east, &south, &west, &center);

    struct ntg_object_measure north_msr = (north != NULL) ?
        ntg_widget_get_measure(north, orient) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure east_msr = (east != NULL) ?
        ntg_widget_get_measure(east, orient) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure south_msr = (south != NULL) ?
        ntg_widget_get_measure(south, orient) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure west_msr = (west != NULL) ?
        ntg_widget_get_measure(west, orient) :
        (struct ntg_object_measure) {0};
    struct ntg_object_measure center_msr = (center != NULL) ?
        ntg_widget_get_measure(center, orient) :
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
        ntg_widget_size_map_set(out_size_map, north, north_size);
    if(east != NULL)
        ntg_widget_size_map_set(out_size_map, east, east_size);
    if(south != NULL)
        ntg_widget_size_map_set(out_size_map, south, south_size);
    if(west != NULL)
        ntg_widget_size_map_set(out_size_map, west, west_size);
    if(center != NULL)
        ntg_widget_size_map_set(out_size_map, center, center_size);
}

void _ntg_border_box_arrange_fn(
        const ntg_widget* _border_box,
        void* _layout_data,
        ntg_widget_xy_map* out_pos_map,
        sarena* arena)
{
    const ntg_border_box* border_box = (const ntg_border_box*)_border_box;
    struct ntg_xy size = ntg_widget_get_cont_size(_border_box);

    ntg_widget *north, *east, *south, *west, *center;
    get_children(border_box, &north, &east, &south, &west, &center);

    struct ntg_xy north_size = (north != NULL) ?
        ntg_widget_get_size(north) : ntg_xy(0, 0);
    struct ntg_xy east_size = (east != NULL) ?
        ntg_widget_get_size(east) : ntg_xy(0, 0);
    struct ntg_xy south_size = (south != NULL) ?
        ntg_widget_get_size(south) : ntg_xy(0, 0);
    struct ntg_xy west_size = (west != NULL) ?
        ntg_widget_get_size(west) : ntg_xy(0, 0);

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
        ntg_widget_xy_map_set(out_pos_map, north, north_pos);
    if(east != NULL)
        ntg_widget_xy_map_set(out_pos_map, east, east_pos);
    if(south != NULL)
        ntg_widget_xy_map_set(out_pos_map, south, south_pos);
    if(west != NULL)
        ntg_widget_xy_map_set(out_pos_map, west, west_pos);
    if(center != NULL)
        ntg_widget_xy_map_set(out_pos_map, center, center_pos);
}

static void 
get_children(const ntg_border_box* box, ntg_widget** out_north,
             ntg_widget** out_east, ntg_widget** out_south,
             ntg_widget** out_west, ntg_widget** out_center)
{
    (*out_north) = box->_children[NTG_BORDER_BOX_NORTH];
    (*out_east) = box->_children[NTG_BORDER_BOX_EAST];
    (*out_south) = box->_children[NTG_BORDER_BOX_SOUTH];
    (*out_west) = box->_children[NTG_BORDER_BOX_WEST];
    (*out_center) = box->_children[NTG_BORDER_BOX_CENTER];
}

static void rm_child_fn(ntg_widget* _border_box, ntg_widget* child)
{
    ntg_border_box* border_box = (ntg_border_box*)_border_box;

    size_t i;
    for(i = 0; i < 5; i++)
    {
        if(border_box->_children[i] == child)
            border_box->_children[i] = NULL;
    }
}
