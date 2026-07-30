#include "ntg.h"
#include <stdlib.h>
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

struct box_layout_cache
{
    struct ntg_object_measure measure[2];
};

static inline size_t calculate_total_spacing(size_t spacing, size_t child_count)
{
    return (child_count > 0) ? ((child_count - 1) * spacing) : 0;
}

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_box_opts ntg_box_opts_def()
{
    return (struct ntg_box_opts) {
        .orient = NTG_ORIENT_H,
        .prim_align = NTG_ALIGN_1,
        .sec_align = NTG_ALIGN_1,
        .spacing = 0,
        .bg = ntg_vcell_def()
    };
}

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

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void ntg_box_init(
        ntg_box* box,
        const struct ntg_box_opts* opts,
        int* out_status)
{
    int _status;

    ntg_box_init_inherit(
            box,
            &NTG_BOX_VTABLE,
            &NTG_TYPE_BOX,
            &_status);

    if(!_status)
        ntg_box_set_opts(box, opts);

    ntg_vreturn(out_status, _status);
}

void ntg_box_deinit(ntg_box* box)
{
    if(!box) return;

    box->_opts = ntg_box_opts_def();
    box->hooks = (struct ntg_box_hooks) {0};

    free(((ntg_object*)box)->layout_cache);
    ((ntg_object*)box)->layout_cache = NULL;

    ntg_object_deinit((ntg_object*)box);
}

void ntg_box_deinit_void(void* _box)
{
    ntg_box_deinit(_box);
}

void ntg_box_set_opts(ntg_box* box, const struct ntg_box_opts* opts)
{
    if(!box) return;

    struct ntg_box_opts old_opts = box->_opts;
    struct ntg_box_opts new_opts = (opts ? (*opts) : ntg_box_opts_def());

    if(ntg_box_opts_are_eql(&old_opts, &new_opts))
        return;

    box->_opts = new_opts;

    ntg_object_set_base_bg(ntg_obj(box), new_opts.bg);

    ntg_object_mark_dirty((ntg_object*)box, NTG_OBJECT_DIRTY_FULL);
}

const struct ntg_object_vec* ntg_box_get_children(const ntg_box* box)
{
    if(!box) return NULL;

    return &(((ntg_object*)box)->_children);
}

void ntg_box_add_child(ntg_box* box, ntg_object* child, int* out_status)
{
    ntg_init_status(out_status);

    if(!box || !child)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    int _status;

    ntg_object_attach((ntg_object*)box, child, &_status);

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

    ntg_object_mark_dirty((ntg_object*)box, NTG_OBJECT_DIRTY_FULL);

    if(box->hooks.on_child_add_fn)
        box->hooks.on_child_add_fn(box, child);
}

void ntg_box_rm_child(ntg_box* box, ntg_object* child)
{
    if(!box || !child) return;

    if(child->_parent != ntg_obj(box))
        return;

    ntg_object_detach(child);

    ntg_object_mark_dirty((ntg_object*)box, NTG_OBJECT_DIRTY_FULL);

    if(box->hooks.on_child_rm_fn)
        box->hooks.on_child_rm_fn(box, child);
}

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void ntg_box_init_inherit(
        ntg_box* box,
        const struct ntg_object_vtable* vtable,
        const ntg_type* type,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!box || !type)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    if(!ntg_type_instance_of(type, &NTG_TYPE_BOX))
        ntg_vreturn(out_status, NTG_ERR_INVALID_TYPE);

    int _status;

    ntg_object_init_inherit((ntg_object*)box, vtable, type, &_status);
    switch(_status)
    {
        case 0:
            break;
        case NTG_ERR_ALLOC_FAIL:
            ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);
        default:
            ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
        
    }

    box->_opts = ntg_box_opts_def();
    box->hooks = (struct ntg_box_hooks) {0};

    ((ntg_object*)box)->layout_cache =
            malloc(sizeof(struct box_layout_cache));
    if(!((ntg_object*)box)->layout_cache)
    {
        ntg_object_deinit((ntg_object*)box);
        ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);
    }
}

struct ntg_object_measure ntg_box_measure_fn(
        const ntg_object* _box,
        ntg_orient orient,
        void* _layout_cache,
        sarena* arena)
{
    const ntg_box* box = (const ntg_box*)_box;
    const ntg_object_vec* children = ntg_box_get_children(box);
    struct box_layout_cache* layout_cache = _layout_cache;

    if(children->size == 0) return (struct ntg_object_measure) {0};

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

        if(orient == box->_opts.orient)
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

    size_t spacing = calculate_total_spacing(box->_opts.spacing, children->size);

     struct ntg_object_measure measure = {
        .min_size = min_size,
        .nat_size = nat_size + spacing,
        .max_size = NTG_SIZE_MAX,
        .grow = 1
    };

    layout_cache->measure[orient] = measure;

    return measure;
}

void ntg_box_constrain_fn(
        const ntg_object* _box,
        ntg_orient orient,
        ntg_object_size_map* out_size_map,
        void* _layout_cache,
        sarena* arena)
{
    const ntg_box* box = (const ntg_box*)_box;
    const ntg_object_vec* children = ntg_box_get_children(box);
    size_t size = ntg_object_get_size_1d_cont(_box, orient);
    struct box_layout_cache* layout_cache = _layout_cache;

    if(children->size == 0) return;

    int _status;

    size_t min_size = layout_cache->measure[orient].min_size;
    size_t nat_size = layout_cache->measure[orient].nat_size;

    size_t extra_size;
    size_t array_size = children->size * sizeof(size_t);
    size_t* caps = (size_t*)sarena_malloc(arena, array_size);
    if(!caps) return;
    size_t* _sizes = (size_t*)sarena_malloc(arena, array_size);
    size_t* grows = NULL;

    const ntg_object* it_child;
    struct ntg_object_measure it_measure;
    size_t i;
    if(orient == box->_opts.orient)
    {
        if(size >= nat_size) 
        {
            grows = (size_t*)sarena_malloc(arena, array_size);
            if(!grows) return;
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
                size_t pref_spacing = calculate_total_spacing(box->_opts.spacing, children->size);
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

        ntg_sap_cap_round_robin(caps, grows, _sizes, extra_size, children->size, arena, &_status);
        if(_status != 0) return;

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
}

void ntg_box_arrange_fn(
        const ntg_object* _box,
        ntg_object_pos_map* out_pos_map,
        void* _layout_cache,
        sarena* arena)
{
    const ntg_box* box = (const ntg_box*)_box;
    const ntg_object_vec* children = ntg_box_get_children(box);
    struct ntg_xy size = ntg_object_get_size_cont(_box);

    if(children->size == 0) return;

    int _status;

    
    ntg_orient orient = box->_opts.orient;
    ntg_align prim_align = box->_opts.prim_align;
    ntg_align sec_align = box->_opts.sec_align;

    size_t i;
    const ntg_object* it_child;
    struct ntg_xy it_size;
    struct ntg_oxy _it_size;

    
    struct ntg_oxy _size = ntg_oxy_from_xy(size, orient);
    struct ntg_oxy _children_size = ntg_oxy(0, 0, orient);
    for(i = 0; i < children->size; i++)
    {
        it_child = children->data[i];

        it_size = it_child->_size;
        _it_size = ntg_oxy_from_xy(it_size, orient);

        _children_size.prim_val += _it_size.prim_val;
        _children_size.sec_val = _max2_size(_children_size.sec_val, _it_size.sec_val);
    }

    
    size_t pref_spacing = calculate_total_spacing(box->_opts.spacing, children->size);
    size_t total_spacing = _min2_size(pref_spacing, _size.prim_val - _children_size.prim_val);

    
    size_t array_size = children->size * sizeof(size_t);

    size_t* spacing_caps = (size_t*)sarena_malloc(arena, array_size);
    if(!spacing_caps) return;
    for(i = 0; i < (children->size - 1); i++)
        spacing_caps[i] = NTG_SIZE_MAX;

    size_t* _spacing_after = (size_t*)sarena_calloc(arena, array_size);
    if(!_spacing_after) return;

    ntg_sap_cap_round_robin(spacing_caps, NULL, _spacing_after,
            total_spacing, children->size - 1, arena, &_status);

    if(_status != 0) return;

    
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

        it_size = it_child->_size;
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
}

void ntg_box_child_rm_fn(ntg_object* _box, ntg_object* child)
{
    ntg_box* box = (ntg_box*)_box;

    ntg_object_mark_dirty((ntg_object*)box, NTG_OBJECT_DIRTY_FULL);
}

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
