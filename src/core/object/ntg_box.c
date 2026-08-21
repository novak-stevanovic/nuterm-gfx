#include "ntg.h"
#include <stdlib.h>
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static inline size_t calculate_total_spacing(size_t spacing, size_t child_count)
{
    return (child_count > 0) ? ((child_count - 1) * spacing) : 0;
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_box_opts ntg_box_opts_default(void)
{
    return (struct ntg_box_opts) {
        .orient = NTG_ORIENT_H,
        .prim_align = NTG_ALIGN_1,
        .sec_align = NTG_ALIGN_1,
        .spacing = 0,
        .bg = ntg_vcell_new_default()
    };
}

/* ------------------------------------------------------ */

bool ntg_box_opts_are_eql(
        const struct ntg_box_opts* opts1,
        const struct ntg_box_opts* opts2)
{
    if(opts1 == opts2)
        return true;

    if(!opts1 || !opts2)
        return false;

    return ((opts1->orient == opts2->orient) &&
            (opts1->prim_align == opts2->prim_align) &&
            (opts1->sec_align == opts2->sec_align) &&
            (opts1->spacing == opts2->spacing) &&
            ntg_vcell_are_eql(opts1->bg, opts2->bg));
}

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

int ntg_box_init(
        ntg_box* box,
        const struct ntg_box_opts* opts)
{
    int _status;

    _status = ntg_box_init_inherit(
            box,
            &NTG_BOX_VTABLE,
            &NTG_TYPE_BOX,
            NULL);

    if(!_status)
        ntg_box_set_opts(box, opts);

    return _status;
}

/* ------------------------------------------------------ */

int ntg_box_deinit(ntg_box* box)
{
    if(!box) return NTG_ERR_INV_ARG;

    box->ro.opts = ntg_box_opts_default();

    ntg_object_deinit((ntg_object*)box);

    return 0;
}

/* ------------------------------------------------------ */

void ntg_box_deinit_void(void* _box)
{
    ntg_box_deinit(_box);
}

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

int ntg_box_set_opts(ntg_box* box, const struct ntg_box_opts* opts)
{
    if(!box) return NTG_ERR_INV_ARG;

    struct ntg_box_opts old_opts = box->ro.opts;
    struct ntg_box_opts new_opts = (opts ? (*opts) : ntg_box_opts_default());

    if(ntg_box_opts_are_eql(&old_opts, &new_opts))
        return 0;

    box->ro.opts = new_opts;

    ntg_object_set_base_bg(ntg_obj(box), new_opts.bg);

    ntg_object_mark_dirty((ntg_object*)box, NTG_OBJECT_DIRTY_FULL);

    return 0;
}

/* ------------------------------------------------------ */
/* CHILDREN */
/* ------------------------------------------------------ */

const struct ntg_objptr_vec* ntg_box_get_children(const ntg_box* box)
{
    if(!box) return NULL;

    return &(((ntg_object*)box)->ro.children);
}

/* ------------------------------------------------------ */

int ntg_box_add_child(ntg_box* box, ntg_object* child)
{
    if(!box || !child)
        return NTG_ERR_INV_ARG;

    int _status = ntg_object_attach((ntg_object*)box, child);
    if(_status != 0)
        return _status;

    ntg_object_mark_dirty((ntg_object*)box, NTG_OBJECT_DIRTY_FULL);

    struct ntg_event_box_chldadd_dt event_dt = { .child = child };
    ntg_event_raise(
            &ntg_obj(box)->ro.event_dlgt,
            ntg_event_new(NTG_EVENT_BOX_CHLDADD, box, &event_dt));

    return 0;
}

/* ------------------------------------------------------ */

int ntg_box_rm_child(ntg_box* box, ntg_object* child)
{
    if(!box || !child) return NTG_ERR_INV_ARG;

    if(child->ro.parent != ntg_obj(box))
        return 0;

    ntg_object_detach(child);

    ntg_object_mark_dirty((ntg_object*)box, NTG_OBJECT_DIRTY_FULL);

    struct ntg_event_box_chldrm_dt event_dt = { .child = child };
    ntg_event_raise(
            &ntg_obj(box)->ro.event_dlgt,
            ntg_event_new(NTG_EVENT_BOX_CHLDRM, box, &event_dt));

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

int ntg_box_init_inherit(
        ntg_box* box,
        const struct ntg_object_vtable* vtable,
        const ntg_type* type,
        struct ntg_object_layout_dt* layout_dt)
{
    if(!box || !type)
        return NTG_ERR_INV_ARG;

    if(!ntg_type_instance_of(type, &NTG_TYPE_BOX))
        return NTG_ERR_INV_TYPE;

    int _status = ntg_object_init_inherit(
            (ntg_object*)box, vtable, type, layout_dt);
    if(_status != 0)
        return _status;

    box->ro.opts = ntg_box_opts_default();
    return 0;
}

/* ------------------------------------------------------ */

int ntg_box_measure_fn(
        const ntg_object* _box,
        struct ntg_object_layout_dt* layout_dt,
        enum ntg_orient orient,
        sarena* arena,
        uint32_t* relayout,
        struct ntg_object_measure* out_measure)
{
    (void)layout_dt;
    (void)arena;
    (void)relayout;
    const ntg_box* box = (const ntg_box*)_box;
    const struct ntg_objptr_vec* children = ntg_box_get_children(box);

    if(!out_measure)
        return NTG_ERR_INV_ARG;

    if(children->size == 0)
    {
        *out_measure = (struct ntg_object_measure) {0};
        return 0;
    }

    size_t min_size = 0, nat_size = 0, max_size = 0;

    size_t i;
    struct ntg_object_measure it_measure;
    const ntg_object* it_child;
    for(i = 0; i < children->size; i++)
    {
        it_child = children->data[i];
        it_measure = ntg_object_get_measure(it_child, orient);

        
        it_measure.nat_size = _max2_size(it_measure.nat_size, 1);
        it_measure.max_size = _max2_size(it_measure.max_size, 1);

        if(orient == box->ro.opts.orient)
        {
            min_size += it_measure.min_size;
            nat_size += it_measure.nat_size;
            max_size += it_measure.max_size;
        }
        else
        {
            min_size = _max2_size(min_size, it_measure.min_size);
            nat_size = _max2_size(nat_size, it_measure.nat_size);
            max_size = _max2_size(max_size, it_measure.max_size);
        }
    }

    size_t spacing = calculate_total_spacing(
            box->ro.opts.spacing, children->size);

     struct ntg_object_measure measure = {
        .min_size = min_size,
        .nat_size = nat_size + spacing,
        .max_size = NTG_SIZE_MAX,
        .grow = 1
    };

    *out_measure = measure;
    return 0;
}

/* ------------------------------------------------------ */

int ntg_box_constrain_fn(
        const ntg_object* _box,
        struct ntg_object_layout_dt* layout_dt,
        enum ntg_orient orient,
        ntg_object_size_map* out_size_map,
        sarena* arena,
        uint32_t* relayout)
{
    (void)layout_dt;
    (void)arena;
    (void)relayout;
    const ntg_box* box = (const ntg_box*)_box;
    const struct ntg_objptr_vec* children = ntg_box_get_children(box);
    size_t size = ntg_object_get_size_1d_cont(_box, orient);

    if(children->size == 0) return 0;
    if(size == 0)
    {
        ntg_object_zero_constrain(_box, out_size_map);
        return 0;
    }

    int _status;

    struct ntg_object_measure cont_measure = ntg_object_get_measure_cont(_box, orient);
    size_t min_size = cont_measure.min_size;
    size_t nat_size = cont_measure.nat_size;

    size_t extra_size;
    size_t array_size = children->size * sizeof(size_t);
    size_t* caps = (size_t*)sarena_malloc(arena, array_size);
    if(!caps)
        return NTG_ERR_ALLOC_FAIL;
    size_t* _sizes = (size_t*)sarena_malloc(arena, array_size);
    if(!_sizes)
        return NTG_ERR_ALLOC_FAIL;
    double* scratch_buffer = (double*)sarena_malloc(
            arena, children->size * sizeof(double));
    if(!scratch_buffer)
        return NTG_ERR_ALLOC_FAIL;
    size_t* grows = NULL;

    const ntg_object* it_child;
    struct ntg_object_measure it_measure;
    size_t i;
    if(orient == box->ro.opts.orient)
    {
        if(size >= nat_size) 
        {
            grows = (size_t*)sarena_malloc(arena, array_size);
            if(!grows)
                return NTG_ERR_ALLOC_FAIL;
            extra_size = size - nat_size; 

            for(i = 0; i < children->size; i++)
            {
                it_child = children->data[i];
                it_measure = ntg_object_get_measure(it_child, orient);

                caps[i] = it_measure.max_size;
                _sizes[i] = it_measure.nat_size;
                grows[i] = it_measure.grow;
            }
        }
        else
        {
            if(size >= min_size) 
            {
                size_t pref_spacing = calculate_total_spacing(
                        box->ro.opts.spacing, children->size);
                extra_size = _sub2_size(size - min_size, pref_spacing);

                for(i = 0; i < children->size; i++)
                {
                    it_child = children->data[i];
                    it_measure = ntg_object_get_measure(it_child, orient);

                    caps[i] = it_measure.nat_size;
                    _sizes[i] = it_measure.min_size;
                }
            }
            else 
            {
                extra_size = size;

                for(i = 0; i < children->size; i++)
                {
                    it_child = children->data[i];
                    it_measure = ntg_object_get_measure(it_child, orient);

                    caps[i] = it_measure.min_size;
                    _sizes[i] = 0;
                }
            }
        }

        _status = ntg_sap_cap_round_robin(
                caps, grows, extra_size, children->size,
                scratch_buffer, _sizes, NULL);
        if(_status != 0)
            return _status;

        for(i = 0; i < children->size; i++)
        {
            it_child = children->data[i];

            ntg_object_size_map_set(out_size_map, it_child, _sizes[i]);
        }
    }
    else
    {
        size_t it_size;

        for(i = 0; i < children->size; i++)
        {
            it_child = children->data[i];
            it_measure = ntg_object_get_measure(it_child, orient);

            it_size =_min2_size(size,
                    (it_measure.grow > 0 ?
                     it_measure.max_size :
                     it_measure.nat_size));
            it_size = (it_size > 0) ? it_size : (size > 0 ? 1 : 0);

            ntg_object_size_map_set(out_size_map, it_child, it_size);
        }
    }

    return 0;
}

/* ------------------------------------------------------ */

int ntg_box_arrange_fn(
        const ntg_object* _box,
        struct ntg_object_layout_dt* layout_dt,
        ntg_object_pos_map* out_pos_map,
        sarena* arena,
        uint32_t* relayout)
{
    (void)layout_dt;
    (void)arena;
    (void)relayout;
    const ntg_box* box = (const ntg_box*)_box;
    const struct ntg_objptr_vec* children = ntg_box_get_children(box);
    struct ntg_xy size = ntg_object_get_size_cont(_box);

    if(children->size == 0) return 0;
    if(ntg_xy_is_zero_any(size))
    {
        ntg_object_zero_arrange(_box, out_pos_map);
        return 0;
    }

    int _status;
    
    enum ntg_orient orient = box->ro.opts.orient;
    enum ntg_align prim_align = box->ro.opts.prim_align;
    enum ntg_align sec_align = box->ro.opts.sec_align;

    size_t i;
    const ntg_object* it_child;
    struct ntg_xy it_size;
    struct ntg_oxy _it_size;
    
    struct ntg_oxy _size = ntg_oxy_from_xy(size, orient);
    struct ntg_oxy _children_size = ntg_oxy(0, 0, orient);
    for(i = 0; i < children->size; i++)
    {
        it_child = children->data[i];

        it_size = it_child->ro.size;
        _it_size = ntg_oxy_from_xy(it_size, orient);

        _children_size.prim_val += _it_size.prim_val;
        _children_size.sec_val = _max2_size(_children_size.sec_val, _it_size.sec_val);
    }
    
    size_t pref_spacing = calculate_total_spacing(
            box->ro.opts.spacing, children->size);
    size_t total_spacing = _min2_size(pref_spacing, _size.prim_val - _children_size.prim_val);

    
    size_t array_size = children->size * sizeof(size_t);

    size_t* spacing_caps = (size_t*)sarena_malloc(arena, array_size);
    if(!spacing_caps)
        return NTG_ERR_ALLOC_FAIL;
    for(i = 0; i < (children->size - 1); i++)
        spacing_caps[i] = NTG_SIZE_MAX;

    size_t* _spacing_after = (size_t*)sarena_calloc(arena, array_size);
    if(!_spacing_after)
        return NTG_ERR_ALLOC_FAIL;
    size_t sap_count = children->size - 1;
    double* scratch_buffer = NULL;
    if(sap_count > 0)
    {
        scratch_buffer = (double*)sarena_malloc(
                arena, sap_count * sizeof(double));
        if(!scratch_buffer)
            return NTG_ERR_ALLOC_FAIL;
    }

    _status = ntg_sap_cap_round_robin(spacing_caps, NULL, total_spacing, sap_count,
            scratch_buffer, _spacing_after, NULL);
    if(_status != 0)
        return _status;
    
    struct ntg_oxy _cont_size = ntg_oxy(
            _children_size.prim_val + total_spacing,
            _children_size.sec_val, orient);
    
    struct ntg_oxy _base_offset = ntg_oxy(
        ntg_align_offset(_cont_size.prim_val, _size.prim_val, prim_align),
        ntg_align_offset(_cont_size.sec_val, _size.sec_val, sec_align),
        orient);

    struct ntg_oxy _it_extra_offset = ntg_oxy(0, 0, orient);
    struct ntg_xy it_pos;
    for(i = 0; i < children->size; i++)
    {
        it_child = children->data[i];

        it_size = it_child->ro.size;
        _it_size = ntg_oxy_from_xy(it_size, orient);

        
        _it_extra_offset.sec_val = ntg_align_offset(
                _cont_size.sec_val,
                _it_size.sec_val,
                sec_align);

        
        it_pos = ntg_xy_add(
                ntg_xy_from_oxy(_base_offset),
                ntg_xy_from_oxy(_it_extra_offset));

        ntg_object_pos_map_set(out_pos_map, it_child, it_pos);

        
        _it_extra_offset.prim_val += (_it_size.prim_val + _spacing_after[i]);
    }

    return 0;
}

/* ------------------------------------------------------ */

void ntg_box_child_rm_fn(ntg_object* _box, ntg_object* child)
{
    (void)child;

    ntg_box* box = (ntg_box*)_box;

    ntg_object_mark_dirty((ntg_object*)box, NTG_OBJECT_DIRTY_FULL);
}

/* ------------------------------------------------------ */

void ntg_box_deinit_fn(ntg_object* _box)
{
    ntg_box_deinit(ntg_box(_box));
}

NTG_API const struct ntg_object_vtable NTG_BOX_VTABLE = {
    .measure_fn = ntg_box_measure_fn,
    .constrain_fn = ntg_box_constrain_fn,
    .arrange_fn = ntg_box_arrange_fn,
    .rm_child_fn = ntg_box_child_rm_fn,
    .deinit_fn = ntg_box_deinit_fn
};
