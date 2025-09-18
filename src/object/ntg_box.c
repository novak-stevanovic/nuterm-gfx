#include <assert.h>

#include "object/ntg_box.h"
#include "base/ntg_sap.h"
#include "object/shared/ntg_object_vec.h"
#include "shared/_ntg_shared.h"

/* -------------------------------------------------------------------------- */

static void __init_default_values(ntg_box* box)
{
    box->__orientation = NTG_ORIENTATION_HORIZONTAL;
    box->__primary_alignment = NTG_ALIGNMENT_1;
    box->__secondary_alignment = NTG_ALIGNMENT_1;
}

void __ntg_box_init__(ntg_box* box, ntg_orientation orientation,
        ntg_alignment primary_alignment, ntg_alignment secondary_alignment)
{
    assert(box != NULL);

    __ntg_object_init__(NTG_OBJECT(box), NTG_OBJECT_WIDGET,
            _ntg_box_measure_fn, _ntg_box_constrain_fn,
            _ntg_box_arrange_children_fn, NULL);

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

void ntg_box_add_child(ntg_box* box, ntg_object* child)
{
    assert(box != NULL);
    assert(child != NULL);

    ntg_object* group_root = ntg_object_get_group_root(child);
    ntg_object* parent = ntg_object_get_parent(child,
            NTG_OBJECT_GET_PARENT_EXCL_DECORATOR);

    assert(parent == NULL);

    _ntg_object_add_child(NTG_OBJECT(box), group_root);
}

void ntg_box_rm_child(ntg_box* box, ntg_object* child)
{
    assert(box != NULL);
    assert(child != NULL);

    ntg_object* group_root = ntg_object_get_group_root(child);
    ntg_object* parent = ntg_object_get_parent(child,
            NTG_OBJECT_GET_PARENT_EXCL_DECORATOR);

    assert(parent == NTG_OBJECT(box));

    _ntg_object_rm_child(parent, group_root);
}

/* -------------------------------------------------------------------------- */

struct ntg_measure_result _ntg_box_measure_fn(const ntg_object* _box,
        ntg_orientation orientation, size_t for_size,
        const ntg_measure_context* context)
{
    ntg_box* box = NTG_BOX(_box);
    const ntg_object_vec* children = ntg_object_get_children(_box);

    if(children->_count == 0) return (struct ntg_measure_result) {0};

    size_t min_size = 0, natural_size = 0, max_size;

    size_t i;
    struct ntg_measure_data it_data;
    ntg_object* it_child;
    for(i = 0; i < children->_count; i++)
    {
        it_child = children->_data[i];
        it_data = ntg_measure_context_get(context, it_child);

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

    return (struct ntg_measure_result) {
        .min_size = min_size,
        .natural_size = natural_size,
        // .max_size = max_size
        .max_size = NTG_SIZE_MAX
    };
}

void _ntg_box_constrain_fn(const ntg_object* _box,
        ntg_orientation orientation, size_t size,
        const ntg_constrain_context* context,
        ntg_constrain_output* out_sizes)
{
    ntg_box* box = NTG_BOX(_box);
    const ntg_object_vec* children = ntg_object_get_children(_box);

    if(children->_count == 0) return;

    size_t min_size = ntg_constrain_context_get_min_size(context);
    size_t natural_size = ntg_constrain_context_get_natural_size(context);

    size_t extra_size;
    size_t* block = malloc(children->_count * sizeof(size_t) * 2);
    assert(block != NULL);
    size_t* caps = &(block[0]);
    size_t* _sizes = &(block[children->_count]);

    ntg_object* it_obj;
    struct ntg_constrain_data it_data;
    struct ntg_constrain_result it_result;
    size_t i;
    if(orientation == box->__orientation)
    {
        if(size >= natural_size) // redistribute extra, capped with max_size
        {
            extra_size = size - natural_size;
            for(i = 0; i < children->_count; i++)
            {
                it_obj = children->_data[i];
                it_data = ntg_constrain_context_get(context, it_obj);

                caps[i] = it_data.max_size;
                _sizes[i] = it_data.natural_size;
            }
        }
        else
        {
            if(size >= min_size) // redistribute extra, capped with natural_size
            {
                extra_size = size - min_size;
                for(i = 0; i < children->_count; i++)
                {
                    it_obj = children->_data[i];
                    it_data = ntg_constrain_context_get(context, it_obj);

                    caps[i] = it_data.natural_size;
                    _sizes[i] = it_data.min_size;
                }
            }
            else // redistribute extra, capped with min_size
            {
                extra_size = size;
                for(i = 0; i < children->_count; i++)
                {
                    it_obj = children->_data[i];
                    it_data = ntg_constrain_context_get(context, it_obj);

                    caps[i] = it_data.min_size;
                    _sizes[i] = 0;
                }
            }
        }

        ntg_sap_cap_round_robin(caps, _sizes, extra_size, children->_count);

        for(i = 0; i < children->_count; i++)
        {
            it_obj = children->_data[i];
            it_data = ntg_constrain_context_get(context, it_obj);

            it_result = (struct ntg_constrain_result) {
                .size = _sizes[i]
            };
            ntg_constrain_output_set(out_sizes, it_obj, it_result);
        }
    }
    else
    {
        for(i = 0; i < children->_count; i++)
        {
            it_obj = children->_data[i];
            it_data = ntg_constrain_context_get(context, it_obj);

            it_result = (struct ntg_constrain_result) {
                .size = _min2_size(size, it_data.max_size)
            };
            ntg_constrain_output_set(out_sizes, it_obj, it_result);
        }
    }

    free(block);
}

void _ntg_box_arrange_children_fn(const ntg_object* _box,
        struct ntg_xy size,
        const ntg_arrange_context* context,
        ntg_arrange_output* out_positions)
{
    ntg_box* box = NTG_BOX(_box);
    const ntg_object_vec* children = ntg_object_get_children(_box);
    ntg_orientation orient = box->__orientation;
    ntg_alignment prim_align = box->__primary_alignment;
    ntg_alignment sec_align = box->__secondary_alignment;

    size_t i;
    ntg_object* it_obj;
    struct ntg_arrange_data it_data;
    struct ntg_oxy _it_size;

    struct ntg_oxy _size = ntg_oxy_from_xy(size, orient);
    struct ntg_oxy _content_size = ntg_oxy(0, 0);
    for(i = 0; i < children->_count; i++)
    {
        it_obj = children->_data[i];

        it_data = ntg_arrange_context_get(context, it_obj);
        _it_size = ntg_oxy_from_xy(it_data.size, orient);

        _content_size.prim_val += _it_size.prim_val;
        _content_size.sec_val = _max2_size(_content_size.sec_val, _it_size.sec_val);
    }

    struct ntg_oxy _base_offset;
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

    struct ntg_arrange_result it_result;
    struct ntg_oxy _it_extra_offset = ntg_oxy(0, 0);
    struct ntg_xy _it_pos;
    for(i = 0; i < children->_count; i++)
    {
        it_obj = children->_data[i];
        it_data = ntg_arrange_context_get(context, it_obj);
        _it_size = ntg_oxy_from_xy(it_data.size, orient);

        if(sec_align == NTG_ALIGNMENT_1)
            _it_extra_offset.sec_val = 0;
        else if(sec_align == NTG_ALIGNMENT_2)
            _it_extra_offset.sec_val = (_content_size.sec_val - _it_size.sec_val) / 2;
        else
            _it_extra_offset.sec_val = (_content_size.sec_val - _it_size.sec_val);

        _it_pos = ntg_xy_add(
                ntg_xy_from_oxy(_base_offset, orient),
                ntg_xy_from_oxy(_it_extra_offset, orient));

        it_result = (struct ntg_arrange_result) {
            .pos = _it_pos
        };

        ntg_arrange_output_set(out_positions, it_obj, it_result);

        _it_extra_offset.prim_val += _it_size.prim_val;
    }
}
