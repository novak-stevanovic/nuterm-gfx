#include "ntg.h"
#include "shared/ntg_shared_internal.h"
#include <string.h>

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static void get_children(
        const ntg_main_panel* panel,
        ntg_object** out_north,
        ntg_object** out_east,
        ntg_object** out_south,
        ntg_object** out_west,
        ntg_object** out_center);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

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

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

int ntg_main_panel_init(ntg_main_panel* panel, const struct ntg_main_panel_opts* opts)
{
    if(!panel) return NTG_ERR_INV_ARG;

    int status = ntg_main_panel_init_inherit(
            panel,
            &NTG_MAIN_PANEL_VTABLE,
            &NTG_TYPE_MAIN_PANEL,
            NULL);
    NTG_POST_INHERIT_CHECK(status);

    ntg_main_panel_set_opts(panel, opts);

    return status;
}

/* ------------------------------------------------------ */

int ntg_main_panel_deinit(ntg_main_panel* panel)
{
    if(!panel) return NTG_ERR_INV_ARG;

    ntg_entity_zero(panel);

    ntg_object_deinit((ntg_object*)panel);

    return 0;
}

/* ------------------------------------------------------ */
/* CHILDREN */
/* ------------------------------------------------------ */

int ntg_main_panel_set(
        ntg_main_panel* panel,
        ntg_object* object,
        enum ntg_main_panel_pos pos)
{
    if(!panel || (pos < NTG_MAIN_PANEL_NORTH) || (pos > NTG_MAIN_PANEL_CENTER))
        return NTG_ERR_INV_ARG;

    ntg_object* old_child = panel->ro.children[pos];

    if(old_child == object) return 0;

    if(old_child)
        ntg_object_detach(old_child);

    panel->ro.children[pos] = NULL;

    if(object)
    {
        int _status = ntg_object_attach((ntg_object*)panel, object);
        if(_status != 0)
            return _status;

        panel->ro.children[pos] = object;
    }

    ntg_object_mark_dirty((ntg_object*)panel, NTG_OBJECT_DIRTY_FULL);

    struct ntg_event_main_panel_chldchg_dt event_dt = {
        .old_child = old_child,
        .new_child = object,
        .pos = pos
    };
    ntg_entity_event_raise(ntg_ent(panel), NTG_EVENT_MAIN_PANEL_CHLDCHG, &event_dt);

    return 0;
}

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

int ntg_main_panel_set_opts(
        ntg_main_panel* panel,
        const struct ntg_main_panel_opts* opts)
{
    if(!panel) return NTG_ERR_INV_ARG;

    struct ntg_main_panel_opts old_opts = panel->ro.opts;
    struct ntg_main_panel_opts new_opts = (opts ? (*opts) : NTG_MAIN_PANEL_OPTS_ZERO);

    if(ntg_main_panel_opts_are_eql(&old_opts, &new_opts))
        return 0;

    panel->ro.opts = new_opts;

    ntg_object_set_base_bg(ntg_obj(panel), new_opts.bg);

    struct ntg_event_main_panel_optchg_dt event_dt = {
        .old_opts = &old_opts,
        .new_opts = &new_opts
    };
    ntg_entity_event_raise(ntg_ent(panel), NTG_EVENT_MAIN_PANEL_OPTCHG, &event_dt);

    return 0;
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg_main_panel_init_inherit(
        ntg_main_panel* panel,
        const struct ntg_main_panel_vtable* vtable,
        const ntg_type* type,
        struct ntg_object_layout_dt* layout_dt)
{
    if(!panel || !type || !vtable)
        return NTG_ERR_INV_ARG;

    if(!ntg_type_instanceof(type, &NTG_TYPE_MAIN_PANEL))
        return NTG_ERR_BAD_TYPE;

    int status = ntg_object_init_inherit(
            ntg_obj(panel),
            &vtable->base,
            type,
            layout_dt);
    NTG_POST_INHERIT_CHECK_VTABLE(status);

    ntg_entity_zero(panel);

    return 0;
}

/* ------------------------------------------------------ */
/* IMPLEMENT */
/* ------------------------------------------------------ */

const struct ntg_main_panel_vtable NTG_MAIN_PANEL_VTABLE = {
    .base = {
        .base = {
            .deinit_fn = ntg_main_panel_deinit_fn
        },
        .measure_fn = ntg_main_panel_measure_fn,
        .constrain_fn = ntg_main_panel_constrain_fn,
        .arrange_fn = ntg_main_panel_arrange_fn,
        .rm_child_fn = ntg_main_panel_child_rm_fn
    }
};

int ntg_main_panel_measure_fn(
        const ntg_object* _panel,
        struct ntg_object_layout_dt* layout_dt,
        enum ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        struct ntg_object_measure* out_measure)
{
    (void)layout_dt;
    (void)arena;
    (void)relayout;
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

    if(!out_measure)
        return NTG_ERR_INV_ARG;

    *out_measure = (struct ntg_object_measure) {
        .min_size = min,
        .nat_size = nat,
        .max_size = max,
        .grow = 1
    };
    return 0;
}

/* ------------------------------------------------------ */

int ntg_main_panel_constrain_fn(
        const ntg_object* _panel,
        struct ntg_object_layout_dt* layout_dt,
        enum ntg_orient orient,
        ntg_object_size_map* out_size_map,
        sarena* arena,
        uint32_t* relayout)
{
    (void)layout_dt;
    (void)arena;
    (void)relayout;
    const ntg_main_panel* main_panel = (const ntg_main_panel*)_panel;
    size_t size = ntg_object_get_size_1d_cont(_panel, orient);

    if(_panel->ro.children.size == 0) return 0;
    if(size == 0)
    {
        ntg_object_zero_constrain(_panel, out_size_map);
        return 0;
    }

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
    double scratch_buffer[3];
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
            _status = ntg_sap_cap_round_robin(
                    caps, grows, extra_size, 3, scratch_buffer, _sizes, NULL);
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
            _status = ntg_sap_cap_round_robin(
                    caps, NULL, extra_size, 3, scratch_buffer, _sizes, NULL);
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
            _status = ntg_sap_cap_round_robin(
                    caps, NULL, extra_size, 3, scratch_buffer, _sizes, NULL);
        }

        if(_status != 0)
            return _status;

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
            _status = ntg_sap_cap_round_robin(
                    caps, grows, extra_size, 3, scratch_buffer, _sizes, NULL);

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
            _status = ntg_sap_cap_round_robin(
                    caps, NULL, extra_size, 3, scratch_buffer, _sizes, NULL);
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
            _status = ntg_sap_cap_round_robin(
                    caps, NULL, extra_size, 3, scratch_buffer, _sizes, NULL);
        }

        if(_status != 0)
            return _status;

        size_t alloced_size = _sizes[0] + _sizes[1] + _sizes[2];
        if(alloced_size < size) 
            _sizes[1] = size - _sizes[0] - _sizes[2];

        north_size = _sizes[0];
        south_size = _sizes[2];

        east_size = _sizes[1];
        west_size = _sizes[1];
        center_size = _sizes[1];
    }

    if(north)
        ntg_object_size_map_set(out_size_map, north, north_size);
    if(east)
        ntg_object_size_map_set(out_size_map, east, east_size);
    if(south)
        ntg_object_size_map_set(out_size_map, south, south_size);
    if(west)
        ntg_object_size_map_set(out_size_map, west, west_size);
    if(center)
        ntg_object_size_map_set(out_size_map, center, center_size);

    return 0;
}

/* ------------------------------------------------------ */

int ntg_main_panel_arrange_fn(
        const ntg_object* _panel,
        struct ntg_object_layout_dt* layout_dt,
        ntg_object_pos_map* out_pos_map,
        sarena* arena,
        uint32_t* relayout)
{
    (void)layout_dt;
    (void)arena;
    (void)relayout;
    const ntg_main_panel* main_panel = (const ntg_main_panel*)_panel;
    struct ntg_xy size = ntg_object_get_size_cont(_panel);

    ntg_object *north, *east, *south, *west, *center;
    get_children(main_panel, &north, &east, &south, &west, &center);

    if(_panel->ro.children.size == 0) return 0;
    if(ntg_xy_is_zero_any(size))
    {
        ntg_object_zero_arrange(_panel, out_pos_map);
        return 0;
    }

    struct ntg_xy north_size = (north != NULL) ? north->ro.size : ntg_xy(0, 0);
    struct ntg_xy east_size = (east != NULL) ? east->ro.size : ntg_xy(0, 0);
    struct ntg_xy south_size = (south != NULL) ? south->ro.size : ntg_xy(0, 0);
    struct ntg_xy west_size = (west != NULL) ? west->ro.size : ntg_xy(0, 0);

    size_t west_east_width = west_size.x + east_size.x;
    size_t north_south_height = north_size.y + south_size.y;
    struct ntg_xy collective_side_size = ntg_xy(west_east_width, north_south_height);

    struct ntg_xy center_size = ntg_xy_size(ntg_xy_sub(size, collective_side_size));

    struct ntg_xy north_pos = ntg_xy(0, 0);
    struct ntg_xy east_pos = ntg_xy(west_size.x + center_size.x, north_size.y);
    struct ntg_xy south_pos = ntg_xy(0, north_size.y + center_size.y);
    struct ntg_xy west_pos = ntg_xy(0, north_size.y);
    struct ntg_xy center_pos = ntg_xy(west_size.x, north_size.y);

    if(north)
        ntg_object_pos_map_set(out_pos_map, north, north_pos);
    if(east)
        ntg_object_pos_map_set(out_pos_map, east, east_pos);
    if(south)
        ntg_object_pos_map_set(out_pos_map, south, south_pos);
    if(west)
        ntg_object_pos_map_set(out_pos_map, west, west_pos);
    if(center)
        ntg_object_pos_map_set(out_pos_map, center, center_pos);

    return 0;
}

static void get_children(const ntg_main_panel* panel, ntg_object** out_north,
        ntg_object** out_east, ntg_object** out_south, ntg_object** out_west,
        ntg_object** out_center)
{
    (*out_north) = panel->ro.children[NTG_MAIN_PANEL_NORTH];
    (*out_east) = panel->ro.children[NTG_MAIN_PANEL_EAST];
    (*out_south) = panel->ro.children[NTG_MAIN_PANEL_SOUTH];
    (*out_west) = panel->ro.children[NTG_MAIN_PANEL_WEST];
    (*out_center) = panel->ro.children[NTG_MAIN_PANEL_CENTER];
}

/* ------------------------------------------------------ */

void ntg_main_panel_child_rm_fn(ntg_object* _main_panel, ntg_object* child)
{
    ntg_main_panel* main_panel = (ntg_main_panel*)_main_panel;

    size_t i;
    for(i = 0; i < 5; i++)
    {
        if(main_panel->ro.children[i] == child)
            main_panel->ro.children[i] = NULL;
    }

    ntg_object_mark_dirty(_main_panel, NTG_OBJECT_DIRTY_FULL);
}

void ntg_main_panel_deinit_fn(ntg_entity* _panel)
{
    ntg_main_panel_deinit((ntg_main_panel*)_panel);
}
