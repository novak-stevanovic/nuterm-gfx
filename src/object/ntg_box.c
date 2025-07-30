#include <assert.h>
#include <stdlib.h>

#include "base/ntg_event_types.h"
#include "base/ntg_sap.h"
#include "object/ntg_object.h"
#include "object/shared/ntg_object_vec.h"
#include "shared/_ntg_shared.h"
#include "shared/ntg_log.h"
#include "object/ntg_box.h"

static void __ntg_box_on_nsize(ntg_box* box);

static void __ntg_box_on_constrain(ntg_box* box);

static bool __process_key_fn(ntg_object* _box,
        struct nt_key_event key_event)
{
    ntg_box* box = (ntg_box*)_box;

    if(key_event.type == NT_KEY_EVENT_UTF32)
    {
        switch(key_event.utf32_data.codepoint)
        {
            case 'h':
                ntg_box_set_orientation(box, NTG_ORIENTATION_HORIZONTAL);
                return true;
            case 'v':
                ntg_box_set_orientation(box, NTG_ORIENTATION_VERTICAL);
                return true;
            case 'a':
                ntg_box_set_primary_alignment(box, NTG_ALIGNMENT_1);
                return true;
            case 'b':
                ntg_box_set_primary_alignment(box, NTG_ALIGNMENT_2);
                return true;
            case 'c':
                ntg_box_set_primary_alignment(box, NTG_ALIGNMENT_3);
                return true;
        }
    }

    return false;
}

static void __ntg_box_on_nsize(ntg_box* box)
{
    ntg_object* _box = (ntg_object*)box;
    ntg_orientation orientation = box->_orientation;
    const ntg_object_vec* children = _box->_children;

    struct ntg_oxy _size = ntg_oxy(0, 0);;
    struct ntg_xy it_size;
    struct ntg_oxy _it_size;

    size_t i;
    for(i = 0; i < children->_count; i++)
    {
        it_size = ntg_xy_size(ntg_object_get_natural_size(children->_data[i]));
        _it_size = ntg_oxy_from_xy(it_size, orientation);

        _size.prim_val += _it_size.prim_val;
        _size.sec_val = _max2_size(_size.sec_val, _it_size.sec_val);
    }

    box->__box_content_nsize = ntg_xy_from_oxy(_size, orientation);
}

static void __calculate_nsize_fn(ntg_object* _box)
{
    ntg_box* box = (ntg_box*)_box;

    __ntg_box_on_nsize(box);

    struct ntg_xy box_content_nsize = box->__box_content_nsize;

    struct ntg_xy nsize = ntg_xy_add(box_content_nsize,
            ntg_padding_size_sum(box->_padding));

    _ntg_object_set_natural_content_size(_box, nsize);
}

static void __ntg_box_on_constrain(ntg_box* box)
{
    ntg_object* __box = (ntg_object*)box;
    struct ntg_constr constr = ntg_object_get_content_constr(__box);

    struct ntg_xy padding_sum = ntg_padding_size_sum(box->_padding);

    // TODO: test because changed
    struct ntg_xy applied_padding_sum = ntg_xy(
            (padding_sum.x <= constr.max_size.x) ? padding_sum.x : constr.max_size.x,
            (padding_sum.y <= constr.max_size.y) ? padding_sum.y : constr.max_size.y);

    size_t nsizes[2];
    size_t _sizes1[2], _sizes2[2];

    nsizes[0] = box->_padding.north;
    nsizes[1] = box->_padding.south;
    ntg_sap_nsize_round_robin(nsizes, _sizes1, constr.max_size.y, 2);

    nsizes[0] = box->_padding.west;
    nsizes[1] = box->_padding.east;
    ntg_sap_nsize_round_robin(nsizes, _sizes2, constr.max_size.x, 2);

    struct ntg_padding_size applied_padding = (struct ntg_padding_size) {
        .north = _sizes1[0],
        .south = _sizes1[1],
        .west = _sizes2[0],
        .east = _sizes2[1]
    };

    box->__applied_padding = applied_padding;
    box->__box_content_constr = ntg_constr(
            ntg_xy_sub(constr.min_size, applied_padding_sum),
            ntg_xy_sub(constr.max_size, applied_padding_sum));
}


static void __constrain_fn(ntg_object* _box)
{
    ntg_box* box = (ntg_box*)_box;
    ntg_orientation orientation = box->_orientation;
    const ntg_object_vec* children = _box->_children;

    __ntg_box_on_constrain(box);

    size_t i;
    ntg_object* it_child;

    struct ntg_constr content_constr = box->__box_content_constr;
    struct ntg_oxy _content_constr_max = ntg_oxy_from_xy(content_constr.max_size,
            orientation);

    struct ntg_xy content_nsize = box->__box_content_nsize;
    struct ntg_oxy _content_nsize = ntg_oxy_from_xy(content_nsize, orientation);

    struct ntg_xy it_nsize;
    struct ntg_oxy _it_nsize;
    struct ntg_oxy _it_min_size, _it_max_size;
    struct ntg_xy it_min_size, it_max_size;
    if(_content_nsize.prim_val <= _content_constr_max.prim_val)
    {
        for(i = 0; i < children->_count; i++)
        {
            it_child = children->_data[i];
            it_nsize = ntg_object_get_natural_size(it_child);
            _it_nsize = ntg_oxy_from_xy(it_nsize, orientation);

            _it_min_size.prim_val = _it_nsize.prim_val;
            _it_min_size.sec_val = 0;

            _it_max_size.prim_val = _it_nsize.prim_val;
            _it_max_size.sec_val = _content_constr_max.sec_val;

            it_min_size = ntg_xy_from_oxy(_it_min_size, orientation);
            it_max_size = ntg_xy_from_oxy(_it_max_size, orientation);

            _ntg_object_set_constr(it_child, ntg_constr(it_min_size, it_max_size));
        }
    }
    else if((_content_nsize.prim_val <= _content_constr_max.prim_val) &&
            (_content_nsize.sec_val > _content_constr_max.sec_val))
    {
        size_t _sizes[NTG_BOX_MAX_CHILDREN];

        size_t extra = _ssub_size(_content_constr_max.prim_val,
                _content_nsize.prim_val);

        ntg_sap_nsize_round_robin(NULL, _sizes, extra, children->_count);

        for(i = 0; i < children->_count; i++)
        {
            it_child = children->_data[i];

            it_nsize = ntg_object_get_natural_size(it_child);
            _it_nsize = ntg_oxy_from_xy(it_nsize, orientation);

            _it_min_size = ntg_oxy(_it_nsize.prim_val, 0);
            _it_max_size = ntg_oxy(_it_nsize.prim_val + _sizes[i],
                    _content_constr_max.sec_val);

            it_min_size = ntg_xy_from_oxy(_it_min_size, orientation);
            it_max_size = ntg_xy_from_oxy(_it_max_size, orientation);

            _ntg_object_set_constr(it_child, ntg_constr(it_min_size, it_max_size));
        }
    }
    else
    {
        size_t nsizes[NTG_BOX_MAX_CHILDREN];
        size_t _sizes[NTG_BOX_MAX_CHILDREN];

        for(i = 0; i < children->_count; i++)
        {
            it_child = children->_data[i];
            it_nsize = ntg_object_get_natural_size(it_child);
            _it_nsize = ntg_oxy_from_xy(it_nsize, orientation);

            nsizes[i] = _it_nsize.prim_val;
        }

        ntg_sap_nsize_round_robin(nsizes, _sizes,
                _content_constr_max.prim_val,
                children->_count);

        for(i = 0; i < children->_count; i++)
        {
            it_child = children->_data[i];
            _it_min_size = ntg_oxy(_sizes[i], 0);
            _it_max_size = ntg_oxy(_sizes[i], _content_constr_max.sec_val);

            it_min_size = ntg_xy_from_oxy(_it_min_size, orientation);
            it_max_size = ntg_xy_from_oxy(_it_max_size, orientation);

            _ntg_object_set_constr(it_child, ntg_constr(it_min_size, it_max_size));
        }
    }
}

void _ntg_box_on_measure(ntg_box* box)
{
    ntg_object* _box = (ntg_object*)box;
    ntg_orientation orientation = box->_orientation;
    const ntg_object_vec* children = _box->_children;

    struct ntg_oxy _content_size = ntg_oxy(0, 0);
    struct ntg_xy it_size;
    struct ntg_oxy _it_size;
    size_t i;

    for(i = 0; i < _box->_children->_count; i++)
    {
        it_size = ntg_xy_size(ntg_object_get_size(children->_data[i]));
        _it_size = ntg_oxy_from_xy(it_size, orientation);

        _content_size.prim_val += _it_size.prim_val;
        _content_size.sec_val = _max2_size(_it_size.sec_val, _content_size.sec_val);
    }
    struct ntg_xy content_size = ntg_xy_size(
            ntg_xy_from_oxy(_content_size, orientation));

    box->__box_content_size = content_size;

    struct ntg_xy padding_size = ntg_padding_size_sum(box->__applied_padding);
    struct ntg_xy content_padding_size = ntg_xy_add(content_size, padding_size);
    struct ntg_xy extra_padding_size = ntg_xy_sub(
            ntg_object_get_content_constr(_box).min_size,
            content_padding_size);
    struct ntg_xy box_size = ntg_xy_size(
            ntg_xy_add(content_padding_size, extra_padding_size));

    box->__box_content_extra_size = ntg_xy_size(ntg_xy_sub(box_size, padding_size));
}

static void __measure_fn(ntg_object* _box)
{
    ntg_box* box = (ntg_box*)_box;

    _ntg_box_on_measure(box);

    struct ntg_xy padding_size = ntg_padding_size_sum(box->__applied_padding);

    struct ntg_xy size = ntg_xy_size(
            ntg_xy_add(box->__box_content_size, padding_size));

    _ntg_object_set_content_size(_box, size);
}

static inline struct ntg_xy _determine_total_offset(const ntg_box* box,
        struct ntg_xy base_offset, struct ntg_xy child_size, bool first_child)
{
    ntg_orientation orientation = box->_orientation;

    struct ntg_oxy _align_offset;

    struct ntg_xy content_size = box->__box_content_size;
    struct ntg_xy content_box_size = box->__box_content_extra_size;

    struct ntg_oxy _content_size = ntg_oxy_from_xy(content_size, orientation);
    struct ntg_oxy _content_box_size = ntg_oxy_from_xy(content_box_size, orientation);
    struct ntg_oxy _child_size = ntg_oxy_from_xy(child_size, orientation);

    if(box->_primary_alignment == NTG_ALIGNMENT_1)
    {
        _align_offset.prim_val = 0;
    }
    else if(box->_primary_alignment == NTG_ALIGNMENT_2)
    {
        _align_offset.prim_val = (_content_box_size.prim_val / 2) -
            (_content_size.prim_val / 2);
    }
    else
    {
        _align_offset.prim_val = _content_box_size.prim_val -
            _content_size.prim_val;
    }

    if(box->_secondary_alignment == NTG_ALIGNMENT_1)
    {
        _align_offset.sec_val = 0;
    }
    else if(box->_secondary_alignment == NTG_ALIGNMENT_2)
    {
        _align_offset.sec_val = (_content_box_size.sec_val / 2) -
            (_child_size.sec_val / 2);
    }
    else
    {
        _align_offset.sec_val = _content_box_size.sec_val -
            _child_size.sec_val;
    }

    struct ntg_xy align_offset = ntg_xy_from_oxy(_align_offset, orientation);

    struct ntg_xy padding_offset = ntg_padding_size_offset(box->__applied_padding);

    struct ntg_xy padding_align_offset = ntg_xy_add(padding_offset, align_offset);

    return ntg_xy_add(base_offset, padding_align_offset);
}

static void __arrange_fn(ntg_container* _box)
{
    ntg_object* __box = (ntg_object*)_box;
    ntg_box* box = (ntg_box*)_box;
    ntg_orientation orientation = box->_orientation;
    const ntg_object_vec* children = __box->_children;

    size_t prim_base_offset = 0;

    size_t i;
    ntg_object* it_child;
    struct ntg_xy it_size;
    struct ntg_oxy _it_size;
    struct ntg_xy it_base_pos, it_pos;
    struct ntg_oxy _it_base_pos;
    for(i = 0; i < children->_count; i++)
    {
        it_child = children->_data[i];
        it_size = ntg_xy_size(ntg_object_get_size(it_child));
        _it_size = ntg_oxy_from_xy(it_size, orientation);
        _it_base_pos = ntg_oxy(prim_base_offset, 0);

        it_base_pos = ntg_xy_from_oxy(_it_base_pos, orientation);

        it_pos = _determine_total_offset(box, it_base_pos, it_size, (i == 0));
        _ntg_object_set_pos_inside_content(it_child, it_pos);

        prim_base_offset += _it_size.prim_val;
    }
}

static inline void _set_default_values(ntg_box* box)
{
    box->_padding = NTG_PADDING_SIZE_ZERO;
    box->_orientation = NTG_ORIENTATION_HORIZONTAL;
    box->_primary_alignment = NTG_ALIGNMENT_1;
    box->_secondary_alignment = NTG_ALIGNMENT_1;

    box->__applied_padding = NTG_PADDING_SIZE_ZERO;
    box->__box_content_constr = NTG_CONSTR_UNSET;
    box->__box_content_nsize = NTG_XY_UNSET;
    box->__box_content_size = NTG_XY_UNSET;
    box->__box_content_extra_size = NTG_XY_UNSET;
}

void __ntg_box_init__(ntg_box* box,
        ntg_orientation orientation,
        ntg_alignment primary_alignment,
        ntg_alignment secondary_alignment)
{
    __ntg_container_init__(NTG_CONTAINER(box), __calculate_nsize_fn,
            __constrain_fn, __measure_fn, __arrange_fn);

    _ntg_object_set_process_key_fn(NTG_OBJECT(box), __process_key_fn);

    _set_default_values(box);

    box->_orientation = orientation;
    box->_primary_alignment = primary_alignment;
    box->_secondary_alignment = secondary_alignment;
}

void __ntg_box_deinit__(ntg_box* box)
{
    __ntg_container_deinit__((ntg_container*)box);

    _set_default_values(box);
}

ntg_box* ntg_box_new(ntg_orientation orientation,
        ntg_alignment primary_alignment,
        ntg_alignment secondary_alignment)
{
    ntg_box* new = (ntg_box*)malloc(sizeof(struct ntg_box));
    if(new == NULL) return NULL;

    __ntg_box_init__(new, orientation, primary_alignment, secondary_alignment);
    return new;
}

void ntg_box_destroy(ntg_box* box)
{
    __ntg_box_deinit__(box);

    free(box);
}

void ntg_box_add_child(ntg_box* box, ntg_object* object)
{
    // TODO: NTG_BOX_MAX_CHILDREN
    _ntg_object_child_add((ntg_object*)box, object);
}

void ntg_box_set_padding_size(ntg_box* box, struct ntg_padding_size padding)
{
    assert(box != NULL);

    if(ntg_padding_size_are_equal(box->_padding, padding)) return;

    box->_padding = padding;

    ntg_listenable* listenable = _ntg_object_get_listenable(NTG_OBJECT(box));
    ntg_event e1;
    __ntg_event_init__(&e1, NTG_ETYPE_OBJECT_LAYOUT_INVALID, box, NULL);
    ntg_listenable_raise(listenable, &e1);
}

void ntg_box_set_orientation(ntg_box* box, ntg_orientation orientation)
{
    assert(box != NULL);

    if(box->_orientation == orientation) return;

    box->_orientation = orientation;

    ntg_listenable* listenable = _ntg_object_get_listenable(NTG_OBJECT(box));
    ntg_event e1;
    __ntg_event_init__(&e1, NTG_ETYPE_OBJECT_LAYOUT_INVALID, box, NULL);
    ntg_listenable_raise(listenable, &e1);
}

void ntg_box_set_primary_alignment(ntg_box* box, ntg_alignment alignment)
{
    assert(box != NULL);

    if(box->_primary_alignment == alignment) return;

    box->_primary_alignment = alignment;

    ntg_listenable* listenable = _ntg_object_get_listenable(NTG_OBJECT(box));
    ntg_event e1;
    __ntg_event_init__(&e1, NTG_ETYPE_OBJECT_CONTENT_INVALID, box, NULL);
    ntg_listenable_raise(listenable, &e1);
}

void ntg_box_set_secondary_alignment(ntg_box* box, ntg_alignment alignment)
{
    assert(box != NULL);
    
    if(box->_secondary_alignment == alignment) return;

    box->_secondary_alignment = alignment;

    ntg_listenable* listenable = _ntg_object_get_listenable(NTG_OBJECT(box));
    ntg_event e1;
    __ntg_event_init__(&e1, NTG_ETYPE_OBJECT_CONTENT_INVALID, box, NULL);
    ntg_listenable_raise(listenable, &e1);
}

void ntg_box_set_bg_color(ntg_box* box, nt_color color)
{
    assert(box != NULL);

    _ntg_container_set_bg(NTG_CONTAINER(box), color);
}
