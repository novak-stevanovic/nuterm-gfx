#include <assert.h>
#include <stdlib.h>

#include "base/ntg_sap.h"
#include "object/ntg_object.h"
#include "object/shared/ntg_object_vec.h"
#include "shared/_ntg_shared.h"
#include "object/ntg_box.h"

/* pa - primary axis
 * sa - secondary axis */

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
                ntg_box_set_orientation(box, NTG_BOX_ORIENTATION_HORIZONTAL);
                return true;
            case 'v':
                ntg_box_set_orientation(box, NTG_BOX_ORIENTATION_VERTICAL);
                return true;
            case 'a':
                ntg_box_set_primary_alignment(box, NTG_BOX_ALIGNMENT_0);
                return true;
            case 'b':
                ntg_box_set_primary_alignment(box, NTG_BOX_ALIGNMENT_1);
                return true;
            case 'c':
                ntg_box_set_primary_alignment(box, NTG_BOX_ALIGNMENT_2);
                return true;
        }
    }

    return false;
}

static inline size_t _padding_x(struct ntg_box_padding padding)
{
    return padding.west + padding.east;
}

static inline size_t _padding_y(struct ntg_box_padding padding)
{
    return padding.north + padding.south;
}

static inline size_t _prim_axis(ntg_box_orientation ort, struct ntg_xy size)
{
    return (ort == NTG_BOX_ORIENTATION_HORIZONTAL) ? size.x : size.y;
}

static inline size_t _sec_axis(ntg_box_orientation ort, struct ntg_xy size)
{
    return (ort == NTG_BOX_ORIENTATION_HORIZONTAL) ? size.y : size.x;
}

static inline struct ntg_xy _xy_new(ntg_box_orientation ort,
        size_t psize, size_t ssize)
{
    return ntg_xy(
        (ort == NTG_BOX_ORIENTATION_HORIZONTAL) ? psize : ssize,
        (ort == NTG_BOX_ORIENTATION_HORIZONTAL) ? ssize : psize
    );
}

static inline struct ntg_xy _xy_add_prim_axis_val(ntg_box_orientation ort,
        struct ntg_xy size, size_t psize)
{
    struct ntg_xy to_add = NTG_XY_UNSET;

    if(ort == NTG_BOX_ORIENTATION_HORIZONTAL)
        to_add.x += psize;
    else
        to_add.y += psize;

    return ntg_xy_add(size, to_add);
}

static inline struct ntg_xy _xy_sub_prim_axis_val(ntg_box_orientation ort,
        struct ntg_xy size, size_t psize)
{
    struct ntg_xy to_add = NTG_XY_UNSET;

    if(ort == NTG_BOX_ORIENTATION_HORIZONTAL)
        to_add.x += psize;
    else
        to_add.y += psize;

    return ntg_xy_sub(size, to_add);
}

static void __ntg_box_on_nsize(ntg_box* box)
{
    ntg_object* _box = (ntg_object*)box;
    const ntg_object_vec* children = _box->_children;

    size_t psize = 0, ssize = 0;
    struct ntg_xy it_size;
    size_t i;

    for(i = 0; i < children->_count; i++)
    {
        it_size = ntg_object_get_nsize(children->_data[i]);

        ntg_xy_size_(&it_size);

        psize += _prim_axis(box->_orientation, it_size);
        ssize = _max2_size(_sec_axis(box->_orientation, it_size), ssize);
    }

    box->__box_content_nsize = _xy_new(box->_orientation, psize, ssize);
}

static void __ntg_box_on_constrain(ntg_box* box)
{
    if(box == NULL) return;

    ntg_object* __box = (ntg_object*)box;
    struct ntg_constr constr = ntg_object_get_content_constr(__box);

    size_t padding_x = _padding_x(box->_pref_padding);
    size_t padding_y = _padding_y(box->_pref_padding);

    struct ntg_constr box_content_constr = constr;
    struct ntg_box_padding padding = NTG_BOX_PADDING_UNSET;

    if(constr.max_size.x > padding_x)
    {
        box_content_constr.max_size.x -= padding_x;

        box_content_constr.min_size.x =
            (box_content_constr.min_size.x > padding_x) ?
            box_content_constr.min_size.x - padding_x :
            0;

        padding.west = box->_pref_padding.west;
        padding.east = box->_pref_padding.east;
    }
    if(constr.max_size.y > padding_y)
    {
        box_content_constr.max_size.y -= padding_y;

        box_content_constr.min_size.y =
            (box_content_constr.min_size.y > padding_y) ?
            box_content_constr.min_size.y - padding_y :
            0;

        padding.north = box->_pref_padding.north;
        padding.south = box->_pref_padding.south;
    }

    box->__padding = padding;
    box->__box_content_constr = box_content_constr;
}

static void __calculate_nsize_fn(ntg_object* _box)
{
    ntg_box* box = (ntg_box*)_box;

    __ntg_box_on_nsize(box);

    size_t psize = _prim_axis(box->_orientation, box->__box_content_nsize);
    size_t ssize = _sec_axis(box->_orientation, box->__box_content_nsize);

    struct ntg_xy pref_padding_size = ntg_xy(
            _padding_x(box->_pref_padding),
            _padding_y(box->_pref_padding));

    psize += _prim_axis(box->_orientation, pref_padding_size);
    ssize += _sec_axis(box->_orientation, pref_padding_size);

    _ntg_object_set_content_nsize(_box, _xy_new(box->_orientation, psize, ssize));
}

static void __constrain_fn(ntg_object* _box)
{
    ntg_box* box = (ntg_box*)_box;
    const ntg_object_vec* children = _box->_children;

    __ntg_box_on_constrain(box);

    size_t i;
    ntg_object* it_child;

    struct ntg_constr cconstr = box->__box_content_constr;
    struct ntg_xy cnsize = box->__box_content_nsize;

    ntg_box_orientation ort = box->_orientation;

    struct ntg_xy it_nsize;
    if((_prim_axis(ort, cnsize) <= _prim_axis(ort, cconstr.max_size)) &&
            _sec_axis(ort, cnsize) <= _sec_axis(ort, cconstr.max_size))
    {
        for(i = 0; i < children->_count; i++) {
            it_child = children->_data[i];
            it_nsize = ntg_object_get_nsize(it_child);

            _ntg_object_set_constr(it_child, ntg_constr(it_nsize, it_nsize));
        }
    }
    else if((_prim_axis(ort, cnsize) <= _prim_axis(ort, cconstr.max_size)) &&
            _sec_axis(ort, cnsize) > _sec_axis(ort, cconstr.max_size))
    {
        struct ntg_constr base_constr[NTG_BOX_MAX_CHILDREN];
        size_t nsizes[NTG_BOX_MAX_CHILDREN];
        size_t _sizes[NTG_BOX_MAX_CHILDREN];

        struct ntg_xy it_min_size, it_max_size;
        for(i = 0; i < children->_count; i++)
        {
            it_child = children->_data[i];
            it_nsize = ntg_object_get_nsize(it_child);
            nsizes[i] = _prim_axis(ort, it_nsize);

            it_min_size = _xy_new(ort, _prim_axis(ort, it_nsize), 0),

            it_max_size = _xy_new(ort, _prim_axis(ort, it_nsize),
                    _sec_axis(ort, cconstr.max_size));

            base_constr[i] = ntg_constr(it_min_size, it_max_size);
        }

        size_t extra = _ssub_size(_prim_axis(ort, cconstr.max_size),
                _prim_axis(ort, cnsize));

        ntg_sap_nsize_round_robin(nsizes, _sizes, extra, children->_count);

        for(i = 0; i < children->_count; i++)
        {
            it_child = children->_data[i];

            base_constr[i].max_size = _xy_add_prim_axis_val(ort, 
                    base_constr[i].max_size, _sizes[i]);

            _ntg_object_set_constr(it_child, base_constr[i]);
        }
    }
    else
    {
        struct ntg_constr base_constr[NTG_BOX_MAX_CHILDREN];
        size_t nsizes[NTG_BOX_MAX_CHILDREN];
        size_t _sizes[NTG_BOX_MAX_CHILDREN];

        for(i = 0; i < children->_count; i++)
        {
            it_child = children->_data[i];

            it_nsize = ntg_object_get_nsize(it_child);
            nsizes[i] = _prim_axis(ort, it_nsize);

            base_constr[i] = ntg_constr(
                    _xy_new(ort, 0, 0),
                    _xy_new(ort, 0, _sec_axis(ort,
                            cconstr.max_size)));
        }

        ntg_sap_nsize_round_robin(nsizes, _sizes,
                _prim_axis(ort, cconstr.max_size),
                children->_count);

        for(i = 0; i < children->_count; i++)
        {
            it_child = children->_data[i];

            base_constr[i].min_size = _xy_add_prim_axis_val(ort,
                    base_constr[i].min_size, _sizes[i]);

            base_constr[i].max_size = _xy_add_prim_axis_val(ort,
                    base_constr[i].max_size, _sizes[i]);

            _ntg_object_set_constr(it_child, base_constr[i]);
        }
    }
}

void _ntg_box_on_measure(ntg_box* box)
{
    ntg_object* _box = (ntg_object*)box;

    const ntg_object_vec* children = _box->_children;

    size_t psize = 0, ssize = 0;
    struct ntg_xy it_size;
    size_t i;

    for(i = 0; i < _box->_children->_count; i++)
    {
        it_size = ntg_object_get_size(children->_data[i]);

        ntg_xy_size_(&it_size);

        psize += _prim_axis(box->_orientation, it_size);
        ssize = _max2_size(_sec_axis(box->_orientation, it_size), ssize);
    }

    struct ntg_xy content_size = _xy_new(box->_orientation, psize, ssize);
    ntg_xy_size_(&content_size);

    box->__box_content_size = content_size;

    struct ntg_xy padding_size = ntg_xy(
            _padding_x(box->__padding),
            _padding_y(box->__padding));

    struct ntg_xy content_padding_size = ntg_xy_add(content_size, padding_size);

    struct ntg_xy extra_padding_size = ntg_xy_sub(
            ntg_object_get_content_constr(_box).min_size,
            content_padding_size);

    struct ntg_xy box_size = ntg_xy_add(content_padding_size, extra_padding_size);

    ntg_xy_size_(&box_size);

    box->__box_content_extra_size = ntg_xy_sub(box_size, padding_size);
    ntg_xy_size_(&box->__box_content_extra_size);
}

static void __measure_fn(ntg_object* _box)
{
    ntg_box* box = (ntg_box*)_box;

    _ntg_box_on_measure(box);

    struct ntg_xy padding_size = ntg_xy(
            _padding_x(box->__padding),
            _padding_y(box->__padding));

    struct ntg_xy size = ntg_xy_add(box->__box_content_size, padding_size);
    ntg_xy_size_(&size);

    _ntg_object_set_content_size(_box, size);

}

static inline struct ntg_xy _determine_total_offset(const ntg_box* box,
        struct ntg_xy base_offset, struct ntg_xy child_size, bool first_child)
{
    size_t poffset, soffset;

    struct ntg_xy content_size = box->__box_content_size;
    struct ntg_xy content_box_size = box->__box_content_extra_size;

    if(box->_primary_alignment == NTG_BOX_ALIGNMENT_0)
    {
        poffset = 0;
    }
    else if(box->_primary_alignment == NTG_BOX_ALIGNMENT_1)
    {
        poffset = (_prim_axis(box->_orientation, content_box_size) / 2) -
            (_prim_axis(box->_orientation, content_size) / 2);
    }
    else
    {
        poffset = _prim_axis(box->_orientation, content_box_size) -
            _prim_axis(box->_orientation, content_size);
    }

    if(box->_secondary_alignment == NTG_BOX_ALIGNMENT_0)
    {
        soffset = 0;
    }
    else if(box->_secondary_alignment == NTG_BOX_ALIGNMENT_1)
    {
        soffset = (_sec_axis(box->_orientation, content_box_size) / 2) -
            (_sec_axis(box->_orientation, child_size) / 2);
    }
    else
    {
        soffset = _sec_axis(box->_orientation, content_box_size) -
            _sec_axis(box->_orientation, child_size);
    }

    struct ntg_xy padding_offset = ntg_xy(box->__padding.west,
            box->__padding.north);

    struct ntg_xy align_offset = _xy_new(box->_orientation, poffset, soffset);

    struct ntg_xy padding_align_offset = ntg_xy_add(padding_offset, align_offset);

    return ntg_xy_add(base_offset, padding_align_offset);
}

static void __arrange_fn(ntg_container* _box)
{
    ntg_box* box = (ntg_box*)_box;
    const ntg_object_vec* children = ((ntg_object*)_box)->_children;

    size_t i;
    ntg_object* it_child;
    for(i = (NTG_OBJECT(box))->_children->_count; i < children->_count; i++)
    {
        it_child = children->_data[i];
        _ntg_object_set_pos_inside_content(it_child, ntg_xy(0, 0));
    }

    size_t psize = 0;
    struct ntg_xy it_size;

    struct ntg_xy it_base_pos, it_pos;

    for(i = 0; i < (NTG_OBJECT(box))->_children->_count; i++)
    {
        it_base_pos = _xy_new(box->_orientation, psize, 0);
        
        it_size = ntg_object_get_size(children->_data[i]);
        ntg_xy_size_(&it_size);

        it_pos = _determine_total_offset(box, it_base_pos, it_size, (i == 0));
        _ntg_object_set_pos_inside_content(children->_data[i], it_pos);

        psize += _prim_axis(box->_orientation, it_size);
    }
}

static inline void _set_default_values(ntg_box* box)
{
    box->_pref_padding = NTG_BOX_PADDING_UNSET;
    box->_orientation = NTG_BOX_ORIENTATION_HORIZONTAL;
    box->_primary_alignment = NTG_BOX_ALIGNMENT_0;
    box->_secondary_alignment = NTG_BOX_ALIGNMENT_0;

    box->__padding = NTG_BOX_PADDING_UNSET;
    box->__box_content_constr = NTG_CONSTR_UNSET;
    box->__box_content_nsize = NTG_XY_UNSET;
    box->__box_content_size = NTG_XY_UNSET;
    box->__box_content_extra_size = NTG_XY_UNSET;
}

void __ntg_box_init__(ntg_box* box,
        ntg_box_orientation orientation,
        ntg_box_alignment primary_alignment,
        ntg_box_alignment secondary_alignment)
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

ntg_box* ntg_box_new(ntg_box_orientation orientation,
        ntg_box_alignment primary_alignment,
        ntg_box_alignment secondary_alignment)
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

void ntg_box_set_padding(ntg_box* box, struct ntg_box_padding padding)
{
    assert(box != NULL);

    box->_pref_padding = padding;
}

void ntg_box_set_orientation(ntg_box* box, ntg_box_orientation orientation)
{
    assert(box != NULL);

    box->_orientation = orientation;
}

void ntg_box_set_primary_alignment(ntg_box* box, ntg_box_alignment alignment)
{
    assert(box != NULL);

    box->_primary_alignment = alignment;
}

void ntg_box_set_secondary_alignment(ntg_box* box, ntg_box_alignment alignment)
{
    assert(box != NULL);

    box->_secondary_alignment = alignment;
}

void ntg_box_set_bg_color(ntg_box* box, nt_color color)
{
    assert(box != NULL);

    _ntg_container_set_bg(NTG_CONTAINER(box), color);
}
