#include "ntg.h"
#include "shared/_ntg_shared.h"
#include <assert.h>
#include <stdlib.h>

struct ntg_box_opts ntg_box_opts_def()
{
    return (struct ntg_box_opts) {
        .orientation = NTG_ORIENT_H,
        .palignment = NTG_ALIGN_1,
        .salignment = NTG_ALIGN_1,
        .spacing = 0
    };
}

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_box* ntg_box_new(ntg_entity_system* system)
{
    struct ntg_entity_init_data entity_data = {
        .type = &NTG_ENTITY_BOX,
        .deinit_fn = _ntg_box_deinit_fn,
        .system = system
    };

    ntg_box* new = (ntg_box*)ntg_entity_create(entity_data);
    assert(new != NULL);

    return new;
}

void ntg_box_init(ntg_box* box)
{
    assert(box != NULL);

    struct ntg_object_layout_ops object_data = {
        .init_fn = ntg_box_layout_init_fn,
        .deinit_fn = _ntg_box_layout_deinit_fn,
        .measure_fn = _ntg_box_measure_fn,
        .constrain_fn = _ntg_box_constrain_fn,
        .arrange_fn = __ntg_box_arrange_fn,
        .draw_fn = NULL,
    };

    _ntg_object_init((ntg_object*)box, object_data);

    box->_opts = ntg_box_opts_def();
    ntg_object_vec_init(&box->_children);
}

struct ntg_box_opts ntg_box_get_opts(const ntg_box* box)
{
    assert(box != NULL);

    return box->_opts;
}

void ntg_box_set_opts(ntg_box* box, struct ntg_box_opts opts)
{
    assert(box != NULL);
    
    box->_opts = opts;

    _ntg_object_mark_change((ntg_object*)box);
}

void ntg_box_add_child(ntg_box* box, ntg_object* child)
{
    assert(box != NULL);
    assert(child != NULL);

    bool validate = _ntg_object_validate_add_child((ntg_object*)box, child);
    assert(validate);

    _ntg_object_add_child((ntg_object*)box, child);
    ntg_object_vec_add(&box->_children, child);
}

void ntg_box_rm_child(ntg_box* box, ntg_object* child)
{
    assert(box != NULL);
    assert(child != NULL);

    _ntg_object_add_child((ntg_object*)box, child);
    ntg_object_vec_rm(&box->_children, child);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

static inline size_t calculate_total_spacing(size_t spacing, size_t child_count);

void _ntg_box_deinit_fn(ntg_entity* entity)
{
    assert(entity != NULL);

    ntg_box* box = (ntg_box*)entity;

    box->_opts = ntg_box_opts_def();
    ntg_object_vec_deinit(&box->_children);
    _ntg_object_deinit_fn(entity);
}

void* ntg_box_layout_init_fn(const ntg_object* object)
{
    struct ntg_box_ldata* new = (struct ntg_box_ldata*)malloc(
            sizeof(struct ntg_box_ldata));
    assert(new != NULL);

    (*new) = (struct ntg_box_ldata) {0};

    return new;
}

void _ntg_box_layout_deinit_fn(const ntg_object* object, void* layout_data)
{
    free(layout_data);
}

struct ntg_object_measure _ntg_box_measure_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        struct ntg_object_measure_ctx ctx,
        struct ntg_object_measure_out* out,
        void* _layout_data,
        sarena* arena)
{
    const ntg_box* box = (const ntg_box*)object;
    struct ntg_box_ldata* layout_data = (struct ntg_box_ldata*)_layout_data;
    const ntg_object* _box = (const ntg_object*)box;
    struct ntg_const_object_vecv children = ntg_object_get_children(_box);

    if(children.count == 0) return (struct ntg_object_measure) {0};

    size_t min_size = 0, natural_size = 0, max_size = 0;

    size_t i;
    struct ntg_object_measure it_measure;
    const ntg_object* it_child;
    for(i = 0; i < children.count; i++)
    {
        it_child = children.data[i];
        it_measure = ntg_object_measure_map_get(ctx.measures, it_child, false);

        /* Make sure all are drawn */
        it_measure.natural_size = _max2_size(it_measure.natural_size, 1);
        it_measure.max_size = _max2_size(it_measure.max_size, 1);

        if(orientation == box->_opts.orientation)
        {
            min_size += it_measure.min_size;
            natural_size += it_measure.natural_size;
            max_size += it_measure.max_size;
        }
        else
        {
            min_size = _max2_size(min_size, it_measure.min_size);
            natural_size = _max2_size(natural_size, it_measure.natural_size);
            max_size = _max2_size(max_size, it_measure.max_size);
        }
    }

    size_t spacing = calculate_total_spacing(box->_opts.spacing, children.count);

     struct ntg_object_measure measure = {
        .min_size = min_size,
        .natural_size = natural_size + spacing,
        .max_size = NTG_SIZE_MAX,
        .grow = 1
    };

     if(orientation == NTG_ORIENT_H)
         layout_data->hmeasure = measure;
     else
         layout_data->vmeasure = measure;

     return measure;
}

void _ntg_box_constrain_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t size,
        struct ntg_object_constrain_ctx ctx,
        struct ntg_object_constrain_out* out,
        void* _layout_data,
        sarena* arena)
{
    const ntg_box* box = (const ntg_box*)object;
    struct ntg_const_object_vecv children = ntg_object_get_children(object);
    struct ntg_box_ldata* layout_data = (struct ntg_box_ldata*)_layout_data;

    if(children.count == 0) return;

    struct ntg_object_measure content_size = (orientation == NTG_ORIENT_H) ?
        layout_data->hmeasure : layout_data->vmeasure;
    size_t min_size = content_size.min_size;
    size_t natural_size = content_size.natural_size;

    size_t extra_size;
    size_t array_size = children.count * sizeof(size_t);
    size_t* caps = (size_t*)sarena_malloc(arena, array_size);
    assert(caps != NULL);
    size_t* _sizes = (size_t*)sarena_malloc(arena, array_size);
    size_t* grows = NULL;

    const ntg_object* it_child;
    struct ntg_object_measure it_measure;
    size_t i;
    if(orientation == box->_opts.orientation)
    {
        if(size >= natural_size) // redistribute extra, capped with max_size
        {
            grows = (size_t*)sarena_malloc(arena, array_size);
            assert(grows != NULL);
            extra_size = size - natural_size; // spacing is included in natural_size

            for(i = 0; i < children.count; i++)
            {
                it_child = children.data[i];
                it_measure = ntg_object_measure_map_get(ctx.measures, it_child, false);

                caps[i] = it_measure.max_size;
                _sizes[i] = it_measure.natural_size;
                grows[i] = it_measure.grow;
            }
        }
        else
        {
            if(size >= min_size) // redistribute extra, capped with natural_size
            {
                size_t pref_spacing = calculate_total_spacing(box->_opts.spacing, children.count);
                extra_size = _ssub_size(size - min_size, pref_spacing);

                for(i = 0; i < children.count; i++)
                {
                    it_child = children.data[i];
                    it_measure = ntg_object_measure_map_get(ctx.measures, it_child, false);

                    caps[i] = it_measure.natural_size;
                    _sizes[i] = it_measure.min_size;
                }
            }
            else // redistribute all, capped with min_size
            {
                extra_size = size;

                for(i = 0; i < children.count; i++)
                {
                    it_child = children.data[i];
                    it_measure = ntg_object_measure_map_get(ctx.measures, it_child, false);

                    caps[i] = it_measure.min_size;
                    _sizes[i] = 0;
                }
            }
        }

        ntg_sap_cap_round_robin(caps, grows, _sizes, extra_size, children.count, arena);

        for(i = 0; i < children.count; i++)
        {
            it_child = children.data[i];
            it_measure = ntg_object_measure_map_get(ctx.measures, it_child, false);

            ntg_object_size_map_set(out->sizes, it_child, _sizes[i], false);
        }
    }
    else
    {
        size_t it_size;

        for(i = 0; i < children.count; i++)
        {
            it_child = children.data[i];
            it_measure = ntg_object_measure_map_get(ctx.measures, it_child, false);

            it_size =_min2_size(size, 
                    (it_measure.grow > 0 ?
                     it_measure.max_size :
                     it_measure.natural_size));
            it_size = (it_size > 0) ? it_size : 1;

            ntg_object_size_map_set(out->sizes, it_child, it_size, false);
        }
    }

    // free(block);
}

void __ntg_box_arrange_fn(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_arrange_ctx ctx,
        struct ntg_object_arrange_out* out,
        void* layout_data,
        sarena* arena)
{
    const ntg_box* box = (const ntg_box*)object;
    struct ntg_const_object_vecv children = ntg_object_get_children(object);

    if(children.count == 0) return;

    /* Init */
    ntg_orientation orient = box->_opts.orientation;
    ntg_alignment prim_align = box->_opts.palignment;
    ntg_alignment sec_align = box->_opts.salignment;

    size_t i;
    const ntg_object* it_child;
    struct ntg_xy it_size;
    struct ntg_oxy _it_size;

    /* Calculate children size */
    struct ntg_oxy _size = ntg_oxy_from_xy(size, orient);
    struct ntg_oxy _children_size = ntg_oxy(0, 0, orient);
    for(i = 0; i < children.count; i++)
    {
        it_child = children.data[i];

        it_size = ntg_object_xy_map_get(ctx.sizes, it_child, false);
        _it_size = ntg_oxy_from_xy(it_size, orient);

        _children_size.prim_val += _it_size.prim_val;
        _children_size.sec_val = _max2_size(_children_size.sec_val, _it_size.sec_val);
    }

    /* Calculate spacing */
    size_t pref_spacing = calculate_total_spacing(box->_opts.spacing, children.count);
    size_t total_spacing = _min2_size(pref_spacing, _size.prim_val - _children_size.prim_val);

    /* Distribute padding space */
    size_t array_size = children.count * sizeof(size_t);

    size_t* spacing_caps = (size_t*)sarena_malloc(arena, array_size);
    assert(spacing_caps != NULL);
    for(i = 0; i < (children.count - 1); i++)
        spacing_caps[i] = NTG_SIZE_MAX;

    size_t* _spacing_after = (size_t*)sarena_calloc(arena, array_size);
    assert(_spacing_after != NULL);

    ntg_sap_cap_round_robin(spacing_caps, NULL, _spacing_after,
            total_spacing, children.count - 1, arena);

    /* Calculate content size */
    struct ntg_oxy _content_size = ntg_oxy(
            _children_size.prim_val + total_spacing,
            _children_size.sec_val, orient);

    /* Calculate base offset */
    struct ntg_oxy _base_offset = ntg_oxy(0, 0, orient);
    if(prim_align == NTG_ALIGN_1)
        _base_offset.prim_val = 0;
    else if(prim_align == NTG_ALIGN_2)
        _base_offset.prim_val = (_size.prim_val - _content_size.prim_val) / 2;
    else
        _base_offset.prim_val = _size.prim_val - _content_size.prim_val;

    if(sec_align == NTG_ALIGN_1)
        _base_offset.sec_val = 0;
    else if(sec_align == NTG_ALIGN_2)
        _base_offset.sec_val = (_size.sec_val - _content_size.sec_val) / 2;
    else
        _base_offset.sec_val = (_size.sec_val - _content_size.sec_val);

    struct ntg_oxy _it_extra_offset = ntg_oxy(0, 0, orient);
    struct ntg_xy it_pos;
    for(i = 0; i < children.count; i++)
    {
        it_child = children.data[i];

        it_size = ntg_object_xy_map_get(ctx.sizes, it_child, false);
        _it_size = ntg_oxy_from_xy(it_size, orient);

        /* Calculate offset from secondary alignment */
        if(sec_align == NTG_ALIGN_1)
            _it_extra_offset.sec_val = 0;
        else if(sec_align == NTG_ALIGN_2)
            _it_extra_offset.sec_val = (_content_size.sec_val - _it_size.sec_val) / 2;
        else
            _it_extra_offset.sec_val = (_content_size.sec_val - _it_size.sec_val);

        /* Set position of child */
        it_pos = ntg_xy_add(
                ntg_xy_from_oxy(_base_offset),
                ntg_xy_from_oxy(_it_extra_offset));

        ntg_object_xy_map_set(out->positions, it_child, it_pos, false);

        /* Calculate next primary axis position */
        _it_extra_offset.prim_val += (_it_size.prim_val + _spacing_after[i]);
    }
}

static inline size_t calculate_total_spacing(size_t spacing, size_t child_count)
{
    return (child_count > 0) ? ((child_count - 1) * spacing) : 0;
}
