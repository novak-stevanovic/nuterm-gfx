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
        const ntg_panel* panel,
        ntg_widget** out_north,
        ntg_widget** out_east,
        ntg_widget** out_south,
        ntg_widget** out_west,
        ntg_widget** out_center);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

int ntg_panel_init(ntg_panel* panel, const struct ntg_panel_opts* opts)
{
    if(!panel) return NTG_ERR_INV_ARG;

    int status = ntg_panel_init_inherit(
            panel,
            &NTG_PANEL_VTABLE,
            &NTG_TYPE_PANEL,
            NULL);
    NTG_POST_INHERIT_CHECK(status);

    ntg_panel_set_opts(panel, opts);

    return status;
}

/* ------------------------------------------------------ */

int ntg_panel_deinit(ntg_panel* panel)
{
    if(!panel) return NTG_ERR_INV_ARG;

    ntg_object_zero(panel);

    ntg_widget_deinit((ntg_widget*)panel);

    return 0;
}

/* ------------------------------------------------------ */
/* CHILDREN */
/* ------------------------------------------------------ */

int ntg_panel_set(
        ntg_panel* panel,
        ntg_widget* widget,
        enum ntg_panel_pos pos)
{
    if(!panel || (pos < NTG_PANEL_POS_N) || (pos > NTG_PANEL_POS_C))
        return NTG_ERR_INV_ARG;

    ntg_widget* old_child = panel->ro.children[pos];

    if(old_child == widget) return 0;

    if(old_child)
        ntg_widget_detach(old_child);

    panel->ro.children[pos] = NULL;

    if(widget)
    {
        int _status = ntg_widget_attach((ntg_widget*)panel, widget);
        if(_status != 0)
            return _status;

        panel->ro.children[pos] = widget;
    }

    ntg_widget_mark_dirty((ntg_widget*)panel, NTG_WIDGET_DIRTY_FULL);

    if(widget)
    {
        struct ntg_event_panel_chldadd_dt event_dt = {
            .child = widget,
            .pos = pos
        };
        ntg_object_event_raise(
                ntg_obj(panel), NTG_EVENT_PANEL_CHLDADD, &event_dt);
    }

    return 0;
}

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

int ntg_panel_set_opts(
        ntg_panel* panel,
        const struct ntg_panel_opts* opts)
{
    if(!panel) return NTG_ERR_INV_ARG;

    struct ntg_panel_opts opts_final =
            (opts ? (*opts) : NTG_PANEL_OPTS_ZERO);

    if(ntg_vcell_are_eql(panel->ro.opts.bg, opts_final.bg))
        return 0;

    panel->ro.opts.bg = opts_final.bg;

    ntg_widget_set_base_bg(ntg_wgt(panel), opts_final.bg);

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

int ntg_panel_init_inherit(
        ntg_panel* panel,
        const struct ntg_panel_vtable* vtable,
        const ntg_type* type,
        struct ntg_widget_layout_dt* layout_dt)
{
    if(!panel || !type || !vtable)
        return NTG_ERR_INV_ARG;

    if(!ntg_type_instanceof(type, &NTG_TYPE_PANEL))
        return NTG_ERR_BAD_TYPE;

    int status = ntg_widget_init_inherit(
            ntg_wgt(panel),
            &vtable->base,
            type,
            layout_dt);
    NTG_POST_INHERIT_CHECK_VTABLE(status);

    ntg_object_zero(panel);

    return 0;
}

/* ------------------------------------------------------ */
/* IMPLEMENT */
/* ------------------------------------------------------ */

const struct ntg_panel_vtable NTG_PANEL_VTABLE = {
    .base = {
        .base = {
            .deinit_fn = ntg_panel_deinit_fn
        },
        .measure_fn = ntg_panel_measure_fn,
        .constrain_fn = ntg_panel_constrain_fn,
        .arrange_fn = ntg_panel_arrange_fn,
        .rm_child_fn = ntg_panel_child_rm_fn
    }
};

int ntg_panel_measure_fn(
        const ntg_widget* _panel,
        struct ntg_widget_layout_dt* layout_dt,
        enum ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        struct ntg_widget_measure* out_measure)
{
    (void)layout_dt;
    (void)arena;
    (void)relayout;
    const ntg_panel* panel = (const ntg_panel*)_panel;

    ntg_widget *north, *east, *south, *west, *center;
    get_children(panel, &north, &east, &south, &west, &center);

    struct ntg_widget_measure north_msr = (north != NULL) ?
        ntg_widget_get_measure(north, orient) :
        (struct ntg_widget_measure) {0};
    struct ntg_widget_measure east_msr = (east != NULL) ?
        ntg_widget_get_measure(east, orient) :
        (struct ntg_widget_measure) {0};
    struct ntg_widget_measure south_msr = (south != NULL) ?
        ntg_widget_get_measure(south, orient) :
        (struct ntg_widget_measure) {0};
    struct ntg_widget_measure west_msr = (west != NULL) ?
        ntg_widget_get_measure(west, orient) :
        (struct ntg_widget_measure) {0};
    struct ntg_widget_measure center_msr = (center != NULL) ?
        ntg_widget_get_measure(center, orient) :
        (struct ntg_widget_measure) {0};

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

    *out_measure = (struct ntg_widget_measure) {
        .min_size = min,
        .nat_size = nat,
        .max_size = max,
        .grow = 1
    };
    return 0;
}

/* ------------------------------------------------------ */

int ntg_panel_constrain_fn(
        const ntg_widget* _panel,
        struct ntg_widget_layout_dt* layout_dt,
        enum ntg_orient orient,
        ntg_widget_size_map* out_size_map,
        sarena* arena,
        uint32_t* relayout)
{
    (void)layout_dt;
    (void)arena;
    (void)relayout;
    const ntg_panel* panel = (const ntg_panel*)_panel;
    size_t size = ntg_widget_get_size_1d_cont(_panel, orient);

    if(_panel->ro.children.size == 0) return 0;
    if(size == 0)
    {
        ntg_widget_zero_constrain(_panel, out_size_map);
        return 0;
    }

    ntg_widget *north, *east, *south, *west, *center;
    get_children(panel, &north, &east, &south, &west, &center);

    struct ntg_widget_measure north_msr = (north != NULL) ?
        ntg_widget_get_measure(north, orient) :
        (struct ntg_widget_measure) {0};
    struct ntg_widget_measure east_msr = (east != NULL) ?
        ntg_widget_get_measure(east, orient) :
        (struct ntg_widget_measure) {0};
    struct ntg_widget_measure south_msr = (south != NULL) ?
        ntg_widget_get_measure(south, orient) :
        (struct ntg_widget_measure) {0};
    struct ntg_widget_measure west_msr = (west != NULL) ?
        ntg_widget_get_measure(west, orient) :
        (struct ntg_widget_measure) {0};
    struct ntg_widget_measure center_msr = (center != NULL) ?
        ntg_widget_get_measure(center, orient) :
        (struct ntg_widget_measure) {0};

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

        struct ntg_widget_measure wce_msr = {
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
        struct ntg_widget_measure wce_msr = {
            .min_size = _max3_size(west_msr.min_size, center_msr.min_size, east_msr.min_size),
            .nat_size = _max3_size(west_msr.nat_size, center_msr.nat_size, east_msr.nat_size),
            .max_size = _max3_size(west_msr.max_size, center_msr.max_size, east_msr.max_size),
            .grow = _max3_size(west_msr.grow, center_msr.grow, east_msr.grow)
        };

        struct ntg_widget_measure total_msr = {
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
        ntg_widget_size_map_set(out_size_map, north, north_size);
    if(east)
        ntg_widget_size_map_set(out_size_map, east, east_size);
    if(south)
        ntg_widget_size_map_set(out_size_map, south, south_size);
    if(west)
        ntg_widget_size_map_set(out_size_map, west, west_size);
    if(center)
        ntg_widget_size_map_set(out_size_map, center, center_size);

    return 0;
}

/* ------------------------------------------------------ */

int ntg_panel_arrange_fn(
        const ntg_widget* _panel,
        struct ntg_widget_layout_dt* layout_dt,
        ntg_widget_pos_map* out_pos_map,
        sarena* arena,
        uint32_t* relayout)
{
    (void)layout_dt;
    (void)arena;
    (void)relayout;
    const ntg_panel* panel = (const ntg_panel*)_panel;
    struct ntg_xy size = ntg_widget_get_size_cont(_panel);

    ntg_widget *north, *east, *south, *west, *center;
    get_children(panel, &north, &east, &south, &west, &center);

    if(_panel->ro.children.size == 0) return 0;
    if(ntg_xy_is_zero_any(size))
    {
        ntg_widget_zero_arrange(_panel, out_pos_map);
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
        ntg_widget_pos_map_set(out_pos_map, north, north_pos);
    if(east)
        ntg_widget_pos_map_set(out_pos_map, east, east_pos);
    if(south)
        ntg_widget_pos_map_set(out_pos_map, south, south_pos);
    if(west)
        ntg_widget_pos_map_set(out_pos_map, west, west_pos);
    if(center)
        ntg_widget_pos_map_set(out_pos_map, center, center_pos);

    return 0;
}

static void get_children(const ntg_panel* panel, ntg_widget** out_north,
        ntg_widget** out_east, ntg_widget** out_south, ntg_widget** out_west,
        ntg_widget** out_center)
{
    (*out_north) = panel->ro.children[NTG_PANEL_POS_N];
    (*out_east) = panel->ro.children[NTG_PANEL_POS_E];
    (*out_south) = panel->ro.children[NTG_PANEL_POS_S];
    (*out_west) = panel->ro.children[NTG_PANEL_POS_W];
    (*out_center) = panel->ro.children[NTG_PANEL_POS_C];
}

/* ------------------------------------------------------ */

void ntg_panel_child_rm_fn(ntg_widget* _panel, ntg_widget* child)
{
    ntg_panel* panel = (ntg_panel*)_panel;

    size_t i;
    for(i = 0; i < 5; i++)
    {
        if(panel->ro.children[i] == child)
        {
            panel->ro.children[i] = NULL;
            break;
        }
    }

    ntg_widget_mark_dirty(_panel, NTG_WIDGET_DIRTY_FULL);

    struct ntg_event_panel_chldrm_dt event_dt = { .child = child, .pos = i };
    ntg_object_event_raise(ntg_obj(_panel), NTG_EVENT_PANEL_CHLDRM, &event_dt);
}

void ntg_panel_deinit_fn(ntg_object* _panel)
{
    ntg_panel_deinit((ntg_panel*)_panel);
}
