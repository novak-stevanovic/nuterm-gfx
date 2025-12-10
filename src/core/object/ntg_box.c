#include <assert.h>
#include <stdlib.h>

#include "core/object/ntg_box.h"
#include "base/ntg_sap.h"
#include "core/object/shared/ntg_object_types.h"
#include "core/object/shared/ntg_object_vec.h"
#include "core/scene/shared/ntg_drawable_kit.h"
#include "shared/_ntg_shared.h"

static inline size_t __calculate_total_spacing(size_t spacing, size_t child_count);
static struct ntg_measure_out __measure_content(
        const ntg_box* box,
        ntg_orientation orientation,
        const ntg_measure_ctx* ctx);

/* -------------------------------------------------------------------------- */

static void __init_default_values(ntg_box* box)
{
    box->__orientation = NTG_ORIENTATION_H;
    box->__primary_alignment = NTG_ALIGNMENT_1;
    box->__secondary_alignment = NTG_ALIGNMENT_1;
    box->__spacing = 0;
}

void __ntg_box_init__(
        ntg_box* box,
        ntg_orientation orientation,
        ntg_alignment primary_alignment,
        ntg_alignment secondary_alignment,
        ntg_process_key_fn process_key_fn,
        ntg_on_focus_fn on_focus_fn,
        ntg_on_unfocus_fn on_unfocus_fn,
        void* data)
{
    assert(box != NULL);

    __ntg_object_init__(
            (ntg_object*)box,
            NTG_OBJECT_BOX,
            __ntg_box_measure_fn,
            __ntg_box_constrain_fn,
            __ntg_box_arrange_fn,
            NULL,
            process_key_fn,
            on_focus_fn,
            on_unfocus_fn,
            data);

    __init_default_values(box);

    box->__orientation = orientation;
    box->__primary_alignment = primary_alignment;
    box->__secondary_alignment = secondary_alignment;
}

void __ntg_box_deinit__(ntg_box* box)
{
    assert(box != NULL);

    __init_default_values(box);
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

void ntg_box_set_background(ntg_box* box, ntg_cell background)
{
    assert(box != NULL);

    ntg_object* _box = (ntg_object*)box;

    ntg_object_set_background(_box, background);
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

ntg_cell ntg_box_get_background(const ntg_box* box)
{
    assert(box != NULL);

    const ntg_object* _box = (const ntg_object*)box;

    return _box->_background_;
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

struct ntg_measure_out __ntg_box_measure_fn(
        const ntg_drawable* drawable,
        ntg_orientation orientation,
        size_t for_size,
        const ntg_measure_ctx* ctx,
        sarena* arena)
{
    const ntg_object* object = ntg_drawable_user(drawable);
    const ntg_box* box = (const ntg_box*)object;

    return __measure_content(box, orientation, ctx);
}

void __ntg_box_constrain_fn(
        const ntg_drawable* drawable,
        ntg_orientation orientation,
        size_t size,
        const ntg_constrain_ctx* constrain_ctx,
        const ntg_measure_ctx* measure_ctx,
        ntg_constrain_out* out_sizes,
        sarena* arena)
{
    const ntg_object* object = ntg_drawable_user(drawable);
    const ntg_box* box = (const ntg_box*)object;
    const ntg_object_vec* children = ntg_object_get_children(object);

    if(children->_count == 0) return;

    sa_err _saerr;

    struct ntg_measure_out content_size = __measure_content(
            box, orientation, measure_ctx);
    size_t min_size = content_size.min_size;
    size_t natural_size = content_size.natural_size;

    size_t extra_size;
    size_t array_size = children->_count * sizeof(size_t);
    size_t* caps = (size_t*)sarena_malloc(arena, array_size, &_saerr);
    assert(caps != NULL);
    size_t* _sizes = (size_t*)sarena_malloc(arena, array_size, &_saerr);
    size_t* grows = NULL;

    const ntg_object* it_obj;
    const ntg_drawable* it_drawable;
    struct ntg_measure_data it_data;
    struct ntg_constrain_result it_result;
    size_t i;
    if(orientation == box->__orientation)
    {
        if(size >= natural_size) // redistribute extra, capped with max_size
        {
            grows = (size_t*)sarena_malloc(arena, array_size, &_saerr);
            extra_size = size - natural_size; // spacing is included in natural_size

            for(i = 0; i < children->_count; i++)
            {
                it_obj = children->_data[i];
                it_drawable = ntg_object_to_drawable(it_obj);
                it_data = ntg_measure_ctx_get(measure_ctx, it_drawable);

                caps[i] = it_data.max_size;
                _sizes[i] = it_data.natural_size;
                grows[i] = it_data.grow;
            }
        }
        else
        {
            if(size >= min_size) // redistribute extra, capped with natural_size
            {
                size_t pref_spacing = __calculate_total_spacing(
                        box->__spacing, children->_count);
                extra_size = _ssub_size(size - min_size, pref_spacing);

                for(i = 0; i < children->_count; i++)
                {
                    it_obj = children->_data[i];
                    it_drawable = ntg_object_to_drawable(it_obj);
                    it_data = ntg_measure_ctx_get(measure_ctx, it_drawable);

                    caps[i] = it_data.natural_size;
                    _sizes[i] = it_data.min_size;
                }
            }
            else // redistribute all, capped with min_size
            {
                extra_size = size;

                for(i = 0; i < children->_count; i++)
                {
                    it_obj = children->_data[i];
                    it_drawable = ntg_object_to_drawable(it_obj);
                    it_data = ntg_measure_ctx_get(measure_ctx, it_drawable);

                    caps[i] = it_data.min_size;
                    _sizes[i] = 0;
                }
            }
        }

        ntg_sap_cap_round_robin(caps, grows, _sizes, extra_size, children->_count);

        for(i = 0; i < children->_count; i++)
        {
            it_obj = children->_data[i];
            it_drawable = ntg_object_to_drawable(it_obj);
            it_data = ntg_measure_ctx_get(measure_ctx, it_drawable);

            it_result = (struct ntg_constrain_result) {
                .size = _sizes[i]
            };
            ntg_constrain_out_set(out_sizes, it_drawable, it_result);
        }
    }
    else
    {
        size_t it_size;

        for(i = 0; i < children->_count; i++)
        {
            it_obj = children->_data[i];
            it_drawable = ntg_object_to_drawable(it_obj);
            it_data = ntg_measure_ctx_get(measure_ctx, it_drawable);

            it_size =_min2_size(size, 
                    (it_data.grow > 0 ?
                     it_data.max_size :
                     it_data.natural_size));
            it_size = (it_size > 0) ? it_size : 1;

            it_result = (struct ntg_constrain_result) {
                .size = it_size
            };
            ntg_constrain_out_set(out_sizes, it_drawable, it_result);
        }
    }

    // free(block);
}

void __ntg_box_arrange_fn(
        const ntg_drawable* drawable,
        struct ntg_xy size,
        const ntg_arrange_ctx* ctx,
        ntg_arrange_out* out_positions,
        sarena* arena)
{
    const ntg_object* object = ntg_drawable_user(drawable);
    const ntg_box* box = (const ntg_box*)object;
    const ntg_object_vec* children = ntg_object_get_children(object);

    if(children->_count == 0) return;

    ntg_orientation orient = box->__orientation;
    ntg_alignment prim_align = box->__primary_alignment;
    ntg_alignment sec_align = box->__secondary_alignment;

    sa_err _saerr;

    size_t i;
    const ntg_object* it_obj;
    const ntg_drawable* it_drawable;
    struct ntg_arrange_data it_data;
    struct ntg_oxy _it_size;

    struct ntg_oxy _size = ntg_oxy_from_xy(size, orient);
    struct ntg_oxy _children_size = ntg_oxy(0, 0, orient);
    for(i = 0; i < children->_count; i++)
    {
        it_obj = children->_data[i];
        it_drawable = ntg_object_to_drawable(it_obj);

        it_data = ntg_arrange_ctx_get(ctx, it_drawable);
        _it_size = ntg_oxy_from_xy(it_data.size, orient);

        _children_size.prim_val += _it_size.prim_val;
        _children_size.sec_val = _max2_size(_children_size.sec_val, _it_size.sec_val);
    }

    size_t pref_spacing = __calculate_total_spacing(
            box->__spacing, children->_count);
    size_t total_spacing = _min2_size(pref_spacing, _size.prim_val - _children_size.prim_val);

    size_t* spacing_caps = (size_t*)sarena_malloc(arena,
            children->_count * sizeof(size_t), &_saerr);
    assert(spacing_caps != NULL);
    for(i = 0; i < (children->_count - 1); i++)
        spacing_caps[i] = NTG_SIZE_MAX;
    size_t* _spacing_after = (size_t*)sarena_calloc(arena,
            children->_count * sizeof(size_t), &_saerr);
    assert(_spacing_after != NULL);
    ntg_sap_cap_round_robin(spacing_caps, NULL, _spacing_after,
            total_spacing, children->_count - 1);

    struct ntg_oxy _content_size = ntg_oxy(
            _children_size.prim_val + total_spacing,
            _children_size.sec_val, orient);

    size_t prim_offset, sec_offset;
    if(prim_align == NTG_ALIGNMENT_1)
        prim_offset = 0;
    else if(prim_align == NTG_ALIGNMENT_2)
        prim_offset = (_size.prim_val - _content_size.prim_val) / 2;
    else
        prim_offset = _size.prim_val - _content_size.prim_val;

    if(sec_align == NTG_ALIGNMENT_1)
        sec_offset = 0;
    else if(sec_align == NTG_ALIGNMENT_2)
        sec_offset = (_size.sec_val - _content_size.sec_val) / 2;
    else
        sec_offset = (_size.sec_val - _content_size.sec_val);
    struct ntg_oxy _base_offset = ntg_oxy(prim_offset, sec_offset, orient);

    struct ntg_arrange_result it_result;
    struct ntg_oxy _it_extra_offset = ntg_oxy(0, 0, orient);
    struct ntg_xy _it_pos;
    for(i = 0; i < children->_count; i++)
    {
        it_obj = children->_data[i];
        it_drawable = ntg_object_to_drawable(it_obj);

        it_data = ntg_arrange_ctx_get(ctx, it_drawable);
        _it_size = ntg_oxy_from_xy(it_data.size, orient);

        if(sec_align == NTG_ALIGNMENT_1)
            _it_extra_offset.sec_val = 0;
        else if(sec_align == NTG_ALIGNMENT_2)
            _it_extra_offset.sec_val = (_content_size.sec_val - _it_size.sec_val) / 2;
        else
            _it_extra_offset.sec_val = (_content_size.sec_val - _it_size.sec_val);

        _it_pos = ntg_xy_add(
                ntg_xy_from_oxy(_base_offset),
                ntg_xy_from_oxy(_it_extra_offset));

        it_result = (struct ntg_arrange_result) {
            .pos = _it_pos
        };

        ntg_arrange_out_set(out_positions, it_drawable, it_result);

        _it_extra_offset.prim_val += (_it_size.prim_val + _spacing_after[i]);
    }
}

static inline size_t __calculate_total_spacing(size_t spacing, size_t child_count)
{
    return (child_count > 0) ? ((child_count - 1) * spacing) : 0;
}

static struct ntg_measure_out __measure_content(
        const ntg_box* box,
        ntg_orientation orientation,
        const ntg_measure_ctx* ctx)
{
    const ntg_object* _box = (const ntg_object*)box;
    const ntg_object_vec* children = ntg_object_get_children(_box);

    if(children->_count == 0) return (struct ntg_measure_out) {0};

    size_t min_size = 0, natural_size = 0, max_size = 0;

    size_t i;
    struct ntg_measure_data it_data;
    const ntg_object* it_child;
    const ntg_drawable* it_drawable;
    for(i = 0; i < children->_count; i++)
    {
        it_child = children->_data[i];
        it_drawable = ntg_object_to_drawable(it_child);
        it_data = ntg_measure_ctx_get(ctx, it_drawable);

        /* Make sure all are drawn */
        it_data.natural_size = _max2_size(it_data.natural_size, 1);
        it_data.max_size = _max2_size(it_data.max_size, 1);

        if(orientation == box->__orientation)
        {
            min_size += it_data.min_size;
            natural_size += it_data.natural_size;
            max_size += it_data.max_size;
        }
        else
        {
            min_size = _max2_size(min_size, it_data.min_size);
            natural_size = _max2_size(natural_size, it_data.natural_size);
            max_size = _max2_size(max_size, it_data.max_size);
        }
    }

    size_t spacing = __calculate_total_spacing(box->__spacing, children->_count);
    // size_t spacing = 0;

    return (struct ntg_measure_out) {
        .min_size = min_size,
        .natural_size = natural_size + spacing,
        .max_size = NTG_SIZE_MAX
    };
}
