#include <assert.h>
#include <stdlib.h>

#include "core/object/ntg_box.h"
#include "base/ntg_sap.h"
#include "core/object/shared/ntg_object_measure.h"
#include "core/object/shared/ntg_object_measure_map.h"
#include "core/object/shared/ntg_object_size_map.h"
#include "core/object/shared/ntg_object_types.h"
#include "core/object/shared/ntg_object_vec.h"
#include "core/object/shared/ntg_object_xy_map.h"
#include "shared/_ntg_shared.h"
#include "shared/sarena.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */

static void init_default_values(ntg_box* box)
{
    box->__orientation = NTG_ORIENTATION_H;
    box->__primary_alignment = NTG_ALIGNMENT_1;
    box->__secondary_alignment = NTG_ALIGNMENT_1;
    box->__spacing = 0;
}

void _ntg_box_init_(
        ntg_box* box,
        ntg_orientation orientation,
        ntg_alignment primary_alignment,
        ntg_alignment secondary_alignment,
        ntg_object_process_key_fn process_key_fn,
        ntg_object_focus_fn on_focus_fn,
        ntg_object_unfocus_fn on_unfocus_fn,
        ntg_object_deinit_fn deinit_fn,
        void* data)
{
    assert(box != NULL);

    _ntg_object_init_(
            (ntg_object*)box,
            NTG_OBJECT_BOX,
            _ntg_box_measure_fn,
            _ntg_box_constrain_fn,
            __ntg_box_arrange_fn,
            NULL,
            process_key_fn,
            on_focus_fn,
            on_unfocus_fn,
            (deinit_fn != NULL) ? deinit_fn : _ntg_box_deinit_fn,
            data);

    init_default_values(box);

    box->__orientation = orientation;
    box->__primary_alignment = primary_alignment;
    box->__secondary_alignment = secondary_alignment;
}

void _ntg_box_deinit_(ntg_box* box)
{
    assert(box != NULL);

    _ntg_box_deinit_fn((ntg_object*)box);
}

ntg_orientation ntg_box_get_orientation(const ntg_box* box)
{
    assert(box != NULL);

    return box->__orientation;
}

ntg_alignment ntg_box_get_primary_alignment(const ntg_box* box)
{
    assert(box != NULL);

    return box->__primary_alignment;
}

ntg_alignment ntg_box_get_secondary_alignment(const ntg_box* box)
{
    assert(box != NULL);
    
    return box->__secondary_alignment;
}

size_t ntg_box_get_spacing(const ntg_box* box)
{
    assert(box != NULL);
    
    return box->__spacing;
}

void ntg_box_set_orientation(ntg_box* box, ntg_orientation orientation)
{
    assert(box != NULL);

    box->__orientation = orientation;
}

void ntg_box_set_primary_alignment(ntg_box* box, ntg_alignment alignment)
{
    assert(box != NULL);

    box->__primary_alignment = alignment;
}

void ntg_box_set_secondary_alignment(ntg_box* box, ntg_alignment alignment)
{
    assert(box != NULL);

    box->__secondary_alignment = alignment;
}

void ntg_box_set_spacing(ntg_box* box, size_t spacing)
{
    assert(box != NULL);

    box->__spacing = spacing;
}

void ntg_box_add_child(ntg_box* box, ntg_object* child)
{
    assert(box != NULL);
    assert(child != NULL);

    ntg_object* group_root = ntg_object_get_group_root(child);
    ntg_object* parent = ntg_object_get_parent(child, NTG_OBJECT_PARENT_EXCL_DECOR);

    assert(parent == NULL);

    _ntg_object_add_child((ntg_object*)box, group_root);
}

void ntg_box_rm_child(ntg_box* box, ntg_object* child)
{
    assert(box != NULL);
    assert(child != NULL);

    ntg_object* group_root = ntg_object_get_group_root(child);
    ntg_object* parent = ntg_object_get_parent(child, NTG_OBJECT_PARENT_EXCL_DECOR);

    assert(parent == ((ntg_object*)box));

    _ntg_object_rm_child(parent, group_root);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

static inline size_t calculate_total_spacing(size_t spacing, size_t child_count);
static struct ntg_object_measure measure_content(
        const ntg_box* box,
        ntg_orientation orientation,
        const ntg_object_measure_map* measures);

void _ntg_box_deinit_fn(ntg_object* _box)
{
    assert(_box != NULL);

    init_default_values((ntg_box*)_box);
}

struct ntg_object_measure _ntg_box_measure_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        struct ntg_object_measure_ctx ctx,
        struct ntg_object_measure_out* out,
        sarena* arena)
{
    const ntg_box* box = (const ntg_box*)object;

    return measure_content(box, orientation, ctx.measures);
}

void _ntg_box_constrain_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t size,
        struct ntg_object_constrain_ctx ctx,
        struct ntg_object_constrain_out* out,
        sarena* arena)
{
    const ntg_box* box = (const ntg_box*)object;
    const ntg_object_vec* children = ntg_object_get_children(object);

    if(children->_count == 0) return;

    struct ntg_object_measure content_size = measure_content(
            box, orientation, ctx.measures);
    size_t min_size = content_size.min_size;
    size_t natural_size = content_size.natural_size;

    size_t extra_size;
    size_t array_size = children->_count * sizeof(size_t);
    size_t* caps = (size_t*)sarena_malloc(arena, array_size);
    assert(caps != NULL);
    size_t* _sizes = (size_t*)sarena_malloc(arena, array_size);
    size_t* grows = NULL;

    const ntg_object* it_child;
    struct ntg_object_measure it_measure;
    size_t i;
    if(orientation == box->__orientation)
    {
        if(size >= natural_size) // redistribute extra, capped with max_size
        {
            grows = (size_t*)sarena_malloc(arena, array_size);
            assert(grows != NULL);
            extra_size = size - natural_size; // spacing is included in natural_size

            for(i = 0; i < children->_count; i++)
            {
                it_child = children->_data[i];
                it_measure = ntg_object_measure_map_get(ctx.measures, it_child);

                caps[i] = it_measure.max_size;
                _sizes[i] = it_measure.natural_size;
                grows[i] = it_measure.grow;
            }
        }
        else
        {
            if(size >= min_size) // redistribute extra, capped with natural_size
            {
                size_t pref_spacing = calculate_total_spacing(
                        box->__spacing, children->_count);
                extra_size = _ssub_size(size - min_size, pref_spacing);

                for(i = 0; i < children->_count; i++)
                {
                    it_child = children->_data[i];
                    it_measure = ntg_object_measure_map_get(ctx.measures, it_child);

                    caps[i] = it_measure.natural_size;
                    _sizes[i] = it_measure.min_size;
                }
            }
            else // redistribute all, capped with min_size
            {
                extra_size = size;

                for(i = 0; i < children->_count; i++)
                {
                    it_child = children->_data[i];
                    it_measure = ntg_object_measure_map_get(ctx.measures, it_child);

                    caps[i] = it_measure.min_size;
                    _sizes[i] = 0;
                }
            }
        }

        ntg_sap_cap_round_robin(caps, grows, _sizes, extra_size, children->_count);

        for(i = 0; i < children->_count; i++)
        {
            it_child = children->_data[i];
            it_measure = ntg_object_measure_map_get(ctx.measures, it_child);

            ntg_object_size_map_set(out->sizes, it_child, _sizes[i]);
        }
    }
    else
    {
        size_t it_size;

        for(i = 0; i < children->_count; i++)
        {
            it_child = children->_data[i];
            it_measure = ntg_object_measure_map_get(ctx.measures, it_child);

            it_size =_min2_size(size, 
                    (it_measure.grow > 0 ?
                     it_measure.max_size :
                     it_measure.natural_size));
            it_size = (it_size > 0) ? it_size : 1;

            ntg_object_size_map_set(out->sizes, it_child, it_size);
        }
    }

    // free(block);
}

void __ntg_box_arrange_fn(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_arrange_ctx ctx,
        struct ntg_object_arrange_out* out,
        sarena* arena)
{
    const ntg_box* box = (const ntg_box*)object;
    const ntg_object_vec* children = ntg_object_get_children(object);

    if(children->_count == 0) return;

    /* Init */
    ntg_orientation orient = box->__orientation;
    ntg_alignment prim_align = box->__primary_alignment;
    ntg_alignment sec_align = box->__secondary_alignment;

    size_t i;
    const ntg_object* it_child;
    struct ntg_xy it_size;
    struct ntg_oxy _it_size;

    /* Calculate children size */
    struct ntg_oxy _size = ntg_oxy_from_xy(size, orient);
    struct ntg_oxy _children_size = ntg_oxy(0, 0, orient);
    for(i = 0; i < children->_count; i++)
    {
        it_child = children->_data[i];

        it_size = ntg_object_xy_map_get(ctx.sizes, it_child);
        _it_size = ntg_oxy_from_xy(it_size, orient);

        _children_size.prim_val += _it_size.prim_val;
        _children_size.sec_val = _max2_size(_children_size.sec_val, _it_size.sec_val);
    }

    /* Calculate spacing */
    size_t pref_spacing = calculate_total_spacing(
            box->__spacing, children->_count);
    size_t total_spacing = _min2_size(pref_spacing, _size.prim_val - _children_size.prim_val);

    /* Distribute padding space */
    size_t array_size = children->_count * sizeof(size_t);

    size_t* spacing_caps = (size_t*)sarena_malloc(arena, array_size);
    assert(spacing_caps != NULL);
    for(i = 0; i < (children->_count - 1); i++)
        spacing_caps[i] = NTG_SIZE_MAX;

    size_t* _spacing_after = (size_t*)sarena_calloc(arena, array_size);
    assert(_spacing_after != NULL);

    ntg_sap_cap_round_robin(spacing_caps, NULL, _spacing_after,
            total_spacing, children->_count - 1);

    /* Calculate content size */
    struct ntg_oxy _content_size = ntg_oxy(
            _children_size.prim_val + total_spacing,
            _children_size.sec_val, orient);

    /* Calculate base offset */
    struct ntg_oxy _base_offset = ntg_oxy(0, 0, orient);
    if(prim_align == NTG_ALIGNMENT_1)
        _base_offset.prim_val = 0;
    else if(prim_align == NTG_ALIGNMENT_2)
        _base_offset.prim_val = (_size.prim_val - _content_size.prim_val) / 2;
    else
        _base_offset.prim_val = _size.prim_val - _content_size.prim_val;

    if(sec_align == NTG_ALIGNMENT_1)
        _base_offset.sec_val = 0;
    else if(sec_align == NTG_ALIGNMENT_2)
        _base_offset.sec_val = (_size.sec_val - _content_size.sec_val) / 2;
    else
        _base_offset.sec_val = (_size.sec_val - _content_size.sec_val);

    struct ntg_oxy _it_extra_offset = ntg_oxy(0, 0, orient);
    struct ntg_xy it_pos;
    for(i = 0; i < children->_count; i++)
    {
        it_child = children->_data[i];

        it_size = ntg_object_xy_map_get(ctx.sizes, it_child);
        _it_size = ntg_oxy_from_xy(it_size, orient);

        /* Calculate offset from secondary alignment */
        if(sec_align == NTG_ALIGNMENT_1)
            _it_extra_offset.sec_val = 0;
        else if(sec_align == NTG_ALIGNMENT_2)
            _it_extra_offset.sec_val = (_content_size.sec_val - _it_size.sec_val) / 2;
        else
            _it_extra_offset.sec_val = (_content_size.sec_val - _it_size.sec_val);

        /* Set position of child */
        it_pos = ntg_xy_add(
                ntg_xy_from_oxy(_base_offset),
                ntg_xy_from_oxy(_it_extra_offset));

        ntg_object_xy_map_set(out->pos, it_child, it_pos);

        /* Calculate next primary axis position */
        _it_extra_offset.prim_val += (_it_size.prim_val + _spacing_after[i]);
    }
}

static inline size_t calculate_total_spacing(size_t spacing, size_t child_count)
{
    return (child_count > 0) ? ((child_count - 1) * spacing) : 0;
}

static struct ntg_object_measure measure_content(
        const ntg_box* box,
        ntg_orientation orientation,
        const ntg_object_measure_map* measures)
{
    const ntg_object* _box = (const ntg_object*)box;
    const ntg_object_vec* children = ntg_object_get_children(_box);

    if(children->_count == 0) return (struct ntg_object_measure) {0};

    size_t min_size = 0, natural_size = 0, max_size = 0;

    size_t i;
    struct ntg_object_measure it_measure;
    const ntg_object* it_child;
    for(i = 0; i < children->_count; i++)
    {
        it_child = children->_data[i];
        it_measure = ntg_object_measure_map_get(measures, it_child);

        /* Make sure all are drawn */
        it_measure.natural_size = _max2_size(it_measure.natural_size, 1);
        it_measure.max_size = _max2_size(it_measure.max_size, 1);

        if(orientation == box->__orientation)
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

    size_t spacing = calculate_total_spacing(box->__spacing, children->_count);

    return (struct ntg_object_measure) {
        .min_size = min_size,
        .natural_size = natural_size + spacing,
        .max_size = NTG_SIZE_MAX,
        .grow = 1
    };
}
