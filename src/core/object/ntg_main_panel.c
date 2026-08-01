#include "ntg.h"
#include "shared/ntg_shared_internal.h"
#include <string.h>

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

static void get_children(
        const ntg_main_panel* panel,
        ntg_object** out_north,
        ntg_object** out_east,
        ntg_object** out_south,
        ntg_object** out_west,
        ntg_object** out_center);

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_main_panel_opts ntg_main_panel_opts_default()
{
    return (struct ntg_main_panel_opts) {
        .bg = ntg_vcell_new_default()
    };
}

bool ntg_main_panel_opts_are_eql(
        const struct ntg_main_panel_opts* opts1,
        const struct ntg_main_panel_opts* opts2)
{
    if(opts1 == opts2)
        return true;

    if(!opts1 || !opts2)
        return false;

    return ntg_vcell_are_eql(opts1->bg, opts2->bg);
}

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

void ntg_main_panel_init(
        ntg_main_panel* panel,
        const struct ntg_main_panel_opts* opts,
        int* out_status)
{
    int _status;

    ntg_main_panel_init_inherit(
            panel,
            &NTG_MAIN_PANEL_VTABLE,
            &NTG_TYPE_MAIN_PANEL,
            &_status);

    if(_status)
        ntg_main_panel_set_opts(panel, opts);

    ntg_vreturn(out_status, _status);
}

void ntg_main_panel_deinit(ntg_main_panel* panel)
{
    if(!panel) return;

    panel->_opts = ntg_main_panel_opts_default();
    memset(panel->_children, 0, sizeof(panel->_children));
    panel->hooks = (struct ntg_main_panel_hooks) {0};

    ntg_object_deinit((ntg_object*)panel);
}

void ntg_main_panel_deinit_void(void* _panel)
{
    ntg_main_panel_deinit(_panel);
}

/* ------------------------------------------------------ */
/* CHILDREN */
/* ------------------------------------------------------ */

void ntg_main_panel_set(
        ntg_main_panel* panel,
        ntg_object* object,
        enum ntg_main_panel_pos pos,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!panel || (pos < NTG_MAIN_PANEL_NORTH) ||
            (pos > NTG_MAIN_PANEL_CENTER))
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    ntg_object* old_child = panel->_children[pos];

    if(old_child == object) return;

    if(old_child)
        ntg_object_detach(old_child);

    panel->_children[pos] = NULL;

    if(object)
    {
        int _status;
        ntg_object_attach((ntg_object*)panel, object, &_status);
        switch(_status)
        {
            case 0:
                break;
            case NTG_ERR_ALLOC_FAIL:
                ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);
            case NTG_ERR_MAX_CHILDREN:
                ntg_vreturn(out_status, NTG_ERR_MAX_CHILDREN);
            default:
                ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
        }

        panel->_children[pos] = object;
    }

    ntg_object_mark_dirty((ntg_object*)panel, NTG_OBJECT_DIRTY_FULL);

    if(panel->hooks.on_child_chng_fn)
        panel->hooks.on_child_chng_fn(panel, old_child, object, pos);
}

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

void ntg_main_panel_set_opts(
        ntg_main_panel* panel,
        const struct ntg_main_panel_opts* opts)
{
    if(!panel) return;

    struct ntg_main_panel_opts old_opts = panel->_opts;
    struct ntg_main_panel_opts new_opts =
            (opts ? (*opts) : ntg_main_panel_opts_default());

    if(ntg_main_panel_opts_are_eql(&old_opts, &new_opts))
        return;

    panel->_opts = new_opts;

    ntg_object_set_base_bg(ntg_obj(panel), new_opts.bg);

    if(panel->hooks.on_opts_chng_fn)
        panel->hooks.on_opts_chng_fn(panel, &old_opts, &new_opts);
}

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void ntg_main_panel_init_inherit(
        ntg_main_panel* panel,
        const struct ntg_object_vtable* vtable,
        const ntg_type* type,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!panel || !type)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    if(!ntg_type_instance_of(type, &NTG_TYPE_MAIN_PANEL))
        ntg_vreturn(out_status, NTG_ERR_INVALID_TYPE);

    int _status;

    ntg_object_init_inherit((ntg_object*)panel, vtable, type, &_status);
    switch(_status)
    {
        case 0:
            break;
        case NTG_ERR_ALLOC_FAIL:
            ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);
        default:
            ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
        
    }

    panel->_opts = ntg_main_panel_opts_default();
    memset(panel->_children, 0, sizeof(panel->_children));
    panel->hooks = (struct ntg_main_panel_hooks) {0};
}

struct ntg_object_measure ntg_main_panel_measure_fn(
        const ntg_object* _panel,
        ntg_orient orient,
        void* _layout_cache,
        sarena* arena)
{
    const ntg_main_panel* main_panel = (const ntg_main_panel*)_panel;

    ntg_object *north, *east, *south, *west, *center;
    get_children(main_panel, &north, &east, &south, &west, &center);

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

void ntg_main_panel_constrain_fn(
        const ntg_object* _panel,
        ntg_orient orient,
        ntg_object_size_map* out_size_map,
        void* _layout_cache,
        sarena* arena)
{
    const ntg_main_panel* main_panel = (const ntg_main_panel*)_panel;
    size_t size = ntg_object_get_size_1d_cont(_panel, orient);

    ntg_object *north, *east, *south, *west, *center;
    get_children(main_panel, &north, &east, &south, &west, &center);

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

    int _status;

    if(orient == NTG_ORIENT_H)
    {
        north_size = size;
        south_size = size;

        struct ntg_object_measure wce_msr = {
            .min_size = west_msr.min_size + center_msr.min_size + east_msr.min_size,
            .nat_size = west_msr.nat_size + center_msr.nat_size + east_msr.nat_size,
            .max_size = west_msr.max_size + center_msr.max_size + east_msr.max_size,
            .grow = 0 
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

            ntg_sap_cap_round_robin(caps, grows, _sizes, extra_size, 3, arena, &_status);
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

            ntg_sap_cap_round_robin(caps, NULL, _sizes, extra_size, 3, arena, &_status);
        }
        else 
        {
            caps[0] = west_msr.min_size;
            caps[1] = center_msr.min_size;
            caps[2] = east_msr.min_size;
            _sizes[0] = 0;
            _sizes[1] = 0;
            _sizes[2] = 0;
            extra_size = size;

            ntg_sap_cap_round_robin(caps, NULL, _sizes, extra_size, 3, arena, &_status);
        }

        size_t alloced_size = _sizes[0] + _sizes[1] + _sizes[2];
        if(alloced_size < size)
            _sizes[1] = size - _sizes[0] - _sizes[2];

        west_size =  _sizes[0];
        center_size =  _sizes[1];
        east_size =  _sizes[2];
    }
    else 
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
            .grow = 0 
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

            ntg_sap_cap_round_robin(caps, grows, _sizes, extra_size, 3, arena, &_status);

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

            ntg_sap_cap_round_robin(caps, NULL, _sizes, extra_size, 3, arena, &_status);
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

            ntg_sap_cap_round_robin(caps, NULL, _sizes, extra_size, 3, arena, &_status);
        }

        if(_status != 0) return;

        size_t alloced_size = _sizes[0] + _sizes[1] + _sizes[2];
        if(alloced_size < size) 
            _sizes[1] = size - _sizes[0] - _sizes[2];

        north_size = _sizes[0];
        south_size = _sizes[2];

        east_size = _sizes[1];
        west_size = _sizes[1];
        center_size = _sizes[1];
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

void ntg_main_panel_arrange_fn(
        const ntg_object* _panel,
        ntg_object_pos_map* out_pos_map,
        void* _layout_cache,
        sarena* arena)
{
    const ntg_main_panel* main_panel = (const ntg_main_panel*)_panel;
    struct ntg_xy size = ntg_object_get_size_cont(_panel);

    ntg_object *north, *east, *south, *west, *center;
    get_children(main_panel, &north, &east, &south, &west, &center);

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

static void get_children(const ntg_main_panel* panel, ntg_object** out_north,
        ntg_object** out_east, ntg_object** out_south, ntg_object** out_west,
        ntg_object** out_center)
{
    (*out_north) = panel->_children[NTG_MAIN_PANEL_NORTH];
    (*out_east) = panel->_children[NTG_MAIN_PANEL_EAST];
    (*out_south) = panel->_children[NTG_MAIN_PANEL_SOUTH];
    (*out_west) = panel->_children[NTG_MAIN_PANEL_WEST];
    (*out_center) = panel->_children[NTG_MAIN_PANEL_CENTER];
}

void ntg_main_panel_child_rm_fn(ntg_object* _main_panel, ntg_object* child)
{
    ntg_main_panel* main_panel = (ntg_main_panel*)_main_panel;

    size_t i;
    for(i = 0; i < 5; i++)
    {
        if(main_panel->_children[i] == child)
            main_panel->_children[i] = NULL;
    }

    ntg_object_mark_dirty(_main_panel, NTG_OBJECT_DIRTY_FULL);
}

void ntg_main_panel_deinit_fn(ntg_object* _panel)
{
    ntg_main_panel_deinit((ntg_main_panel*)_panel);
}

const struct ntg_object_vtable NTG_MAIN_PANEL_VTABLE = {
    .measure_fn = ntg_main_panel_measure_fn,
    .constrain_fn = ntg_main_panel_constrain_fn,
    .arrange_fn = ntg_main_panel_arrange_fn,
    .rm_child_fn = ntg_main_panel_child_rm_fn,
    .deinit_fn = ntg_main_panel_deinit_fn
};
