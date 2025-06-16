#include <assert.h>
#include <stdlib.h>

#include "base/ntg_sap.h"
#include "object/ntg_object.h"
#include "object/def/ntg_box_def.h"
#include "object/shared/ntg_object_vec.h"
#include "shared/_ntg_shared.h"

/* pa - primary axis
 * sa - secondary axis */

static inline size_t _padding_x(struct ntg_box_padding padding)
{
    return padding.west + padding.east;
}

static inline size_t _padding_y(struct ntg_box_padding padding)
{
    return padding.north + padding.south;
}

static inline void _pa_padding(ntg_box_orientation_t ort,
        struct ntg_box_padding padding, size_t out_ppadding[2])
{
    if(ort == NTG_BOX_ORIENTATION_HORIZONTAL)
    {
        out_ppadding[0] = padding.west;
        out_ppadding[1] = padding.east;
    }
    else
    {
        out_ppadding[0] = padding.north;
        out_ppadding[1] = padding.south;
    }
}

static inline void _sa_padding(ntg_box_orientation_t ort, 
        struct ntg_box_padding padding, size_t out_ppadding[2])
{
    if(ort == NTG_BOX_ORIENTATION_HORIZONTAL)
    {
        out_ppadding[0] = padding.west;
        out_ppadding[1] = padding.east;
    }
    else
    {
        out_ppadding[0] = padding.north;
        out_ppadding[1] = padding.south;
    }
}

static inline size_t _pa_val(ntg_box_orientation_t ort, struct ntg_xy size)
{
    return (ort == NTG_BOX_ORIENTATION_HORIZONTAL) ? size.x : size.y;
}

static inline size_t _sa_val(ntg_box_orientation_t ort, struct ntg_xy size)
{
    return (ort == NTG_BOX_ORIENTATION_HORIZONTAL) ? size.y : size.x;
}

static inline struct ntg_xy _xy_new(ntg_box_orientation_t ort,
        size_t psize, size_t ssize)
{
    return ntg_xy(
        (ort == NTG_BOX_ORIENTATION_HORIZONTAL) ? psize : ssize,
        (ort == NTG_BOX_ORIENTATION_HORIZONTAL) ? ssize : psize
    );
}

static inline struct ntg_xy _xy_add_pa_val(ntg_box_orientation_t ort,
        struct ntg_xy size, size_t psize)
{
    struct ntg_xy to_add = NTG_XY_UNSET;
    
    if(ort == NTG_BOX_ORIENTATION_HORIZONTAL)
        to_add.x += psize;
    else
        to_add.y += psize;

    return ntg_xy_add(size, to_add);
}

void _ntg_box_on_constrain(ntg_box_t* box)
{
    if(box == NULL) return;

    const ntg_object_t* object = (const ntg_object_t*)box;
    struct ntg_constr constr = ntg_object_get_constr(object);

    size_t padding_x = _padding_x(box->_pref_padding);
    size_t padding_y = _padding_y(box->_pref_padding);

    struct ntg_constr content_constr = constr;
    struct ntg_box_padding padding = {0};

    if(constr.max_size.x > padding_x)
    {
        content_constr.max_size.x -= padding_x;

        content_constr.min_size.x =
            (content_constr.min_size.x > padding_x) ?
            content_constr.min_size.x - padding_x :
            0;

        padding.west = box->_pref_padding.west;
        padding.east = box->_pref_padding.east;
    }
    if(constr.max_size.y > padding_y)
    {
        content_constr.max_size.y -= padding_y;

        content_constr.min_size.y =
            (content_constr.min_size.y > padding_y) ?
            content_constr.min_size.y - padding_y :
            0;

        padding.north = box->_pref_padding.north;
        padding.south = box->_pref_padding.south;
    }

    box->_padding = padding;
    box->_cconstr = content_constr;
}

void _ntgbox_on_nsize(ntg_box_t* box)
{
    ntg_object_t* _box = (ntg_object_t*)box;
    const ntg_object_vec_t* children = ntg_object_get_children(_box);

    size_t psize = 0, ssize= 0;
    struct ntg_xy it_size;
    size_t i;

    for(i = 0; i < children->_count; i++)
    {
        it_size = ntg_object_get_nsize(children->_data[i]);

        ntg_xy_size(&it_size);

        psize += _pa_val(box->_ort, it_size);
        ssize = _max2_size(_sa_val(box->_ort, it_size), ssize);
    }

    box->_cnsize = _xy_new(box->_ort, psize, ssize);
}

static void __nsize_fn(ntg_object_t* _box)
{
    ntg_box_t* box = (ntg_box_t*)_box;

    _ntgbox_on_nsize(box);

    size_t psize = _pa_val(box->_ort, box->_cnsize);
    size_t ssize = _sa_val(box->_ort, box->_cnsize);

    size_t padding[2];
    _pa_padding(box->_ort, box->_pref_padding, padding);
    psize += padding[0] + padding[1];

    _sa_padding(box->_ort, box->_pref_padding, padding);
    ssize += padding[0] + padding[1];

    _ntg_object_set_nsize(_box, _xy_new(box->_ort, psize, ssize));
}

static void __constrain_fn(ntg_object_t* _box)
{
    ntg_box_t* box = (ntg_box_t*)_box;
    const ntg_object_vec_t* children = ntg_object_get_children(_box);

    struct ntg_xy nsize = ntg_object_get_nsize(_box);
    // struct ntg_constr constr = ntg_object_get_constr(_box);

    ntg_xy_size(&nsize);

    _ntg_box_on_constrain(box);
    struct ntg_constr cconstr = box->_cconstr;
    struct ntg_xy cnsize = box->_cnsize;

    ntg_object_t* it_child;
    struct ntg_xy it_nsize;
    size_t i;
    if((_pa_val(box->_ort, cnsize) <= _pa_val(box->_ort, cconstr.max_size)) &&
            _sa_val(box->_ort, cnsize) <= _sa_val(box->_ort, cconstr.max_size))
    {
        for(i = 0; i < children->_count; i++) {
            it_child = children->_data[i];
            it_nsize = ntg_object_get_nsize(it_child);

            _ntg_object_set_constr(it_child, ntg_constr(it_nsize, it_nsize));
        }
    }
    else if((_pa_val(box->_ort, cnsize) <= _pa_val(box->_ort, cconstr.max_size)) &&
            _sa_val(box->_ort, cnsize) > _sa_val(box->_ort, cconstr.max_size))
    {
        struct ntg_constr base_constr[NTG_BOX_MAX_CHILDREN];
        size_t nsizes[NTG_BOX_MAX_CHILDREN];
        size_t _sizes[NTG_BOX_MAX_CHILDREN];

        struct ntg_xy it_min_size, it_max_size;
        for(i = 0; i < children->_count; i++)
        {
            it_child = children->_data[i];
            it_nsize = ntg_object_get_nsize(it_child);
            nsizes[i] = _pa_val(box->_ort, it_nsize);

            it_min_size = _xy_new(box->_ort, _pa_val(box->_ort, it_nsize), 0),

            it_max_size = _xy_new(box->_ort, _pa_val(box->_ort, it_nsize),
                    _sa_val(box->_ort, cconstr.max_size));

            base_constr[i] = ntg_constr(it_min_size, it_max_size);
        }

        size_t extra = _ssub_size(_pa_val(box->_ort, cconstr.max_size),
                _pa_val(box->_ort, cnsize));

        ntg_sap_nsize_round_robin(nsizes, _sizes, extra, children->_count);

        for(i = 0; i < children->_count; i++)
        {
            it_child = children->_data[i];

            base_constr[i].max_size = _xy_add_pa_val(box->_ort, 
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
            nsizes[i] = _pa_val(box->_ort, it_nsize);

            base_constr[i] = ntg_constr(
                    ntg_xy(0, 0),
                    _xy_new(box->_ort, 0, _sa_val(box->_ort, cconstr.max_size)));
        }

        ntg_sap_nsize_round_robin(nsizes, _sizes,
                _pa_val(box->_ort, cconstr.max_size),
                children->_count);

        for(i = 0; i < children->_count; i++)
        {
            it_child = children->_data[i];

            base_constr[i].min_size = _xy_add_pa_val(box->_ort,
                    base_constr[i].min_size, _sizes[i]);

            base_constr[i].max_size = _xy_add_pa_val(box->_ort,
                    base_constr[i].max_size, _sizes[i]);

            _ntg_object_set_constr(it_child, base_constr[i]);
        }
    }
}

static void __measure_fn(ntg_object_t* _box)
{
    ntg_box_t* box = (ntg_box_t*)_box;
    const ntg_object_vec_t* children = ntg_object_get_children(_box);

    size_t psize = 0, ssize = 0;
    struct ntg_xy it_size;
    size_t i;

    for(i = 0; i < children->_count; i++)
    {
        it_size = ntg_object_get_size(children->_data[i]);

        ntg_xy_size(&it_size);

        psize += _pa_val(box->_ort, it_size);
        ssize = _max2_size(_sa_val(box->_ort, it_size), ssize);
    }

    struct ntg_xy padding = ntg_xy(_padding_x(box->_padding),
            _padding_y(box->_padding));

    struct ntg_xy size = ntg_xy_add(_xy_new(box->_ort, psize, ssize), padding);
    ntg_xy_size(&size);

    _ntg_object_set_size(_box, size);
}

static void __arrange_fn(ntg_container_t* _box)
{
    ntg_box_t* box = (ntg_box_t*)_box;
    const ntg_object_vec_t* children = ntg_object_get_children((ntg_object_t*)_box);

    size_t psize = 0, ssize = 0;
    struct ntg_xy it_size;
    size_t i;

    struct ntg_xy padding_offset = ntg_xy(box->_padding.west, box->_padding.north);

    struct ntg_xy it_pos;

    for(i = 0; i < children->_count; i++)
    {
        it_pos = ntg_xy_add(_xy_new(box->_ort, psize, 0), padding_offset);
        _ntg_object_set_pos(children->_data[i], it_pos);

        it_size = ntg_object_get_size(children->_data[i]);

        ntg_xy_size(&it_size);

        psize += _pa_val(box->_ort, it_size);
        ssize = _max2_size(_sa_val(box->_ort, it_size), ssize);
    }
}

void __ntg_box_init__(ntg_box_t* box, ntg_box_orientation_t orientation)
{
    __ntg_container_init__((ntg_container_t*)box, __nsize_fn,
            __constrain_fn, __measure_fn, __arrange_fn);

    box->_ort = orientation;

    box->_spacing = 0;
    box->_pref_padding = (struct ntg_box_padding) {0};

    box->_padding = (struct ntg_box_padding) {0};
    box->_cconstr = NTG_CONSTR_UNSET;
    box->_cnsize = NTG_XY_UNSET;
}

void __ntg_box_deinit__(ntg_box_t* box)
{
    __ntg_container_deinit__((ntg_container_t*)box);

    box->_pref_padding = (struct ntg_box_padding) {0};
    box->_spacing = 0;

    box->_padding = (struct ntg_box_padding) {0};
    box->_cconstr = NTG_CONSTR_UNSET;
    box->_cnsize = NTG_XY_UNSET;
}

ntg_box_t* ntg_box_new(ntg_box_orientation_t orientation)
{
    ntg_box_t* new = (ntg_box_t*)malloc(sizeof(struct ntg_box));
    if(new == NULL) return NULL;

    __ntg_box_init__(new, orientation);
    return new;
}

void ntg_box_destroy(ntg_box_t* box)
{
    if(box == NULL) return;

    __ntg_box_deinit__(box);
    free(box);
}

void ntg_box_add_child(ntg_box_t* box, ntg_object_t* object)
{
    // TODO: NTG_BOX_MAX_CHILDREN
    _ntg_object_child_add((ntg_object_t*)box, object);
}

void ntg_box_set_padding(ntg_box_t* box, struct ntg_box_padding padding)
{
    assert(box != NULL);

    box->_pref_padding = padding;
}

struct ntg_box_padding ntg_box_get_padding(const ntg_box_t* box)
{
    assert(box != NULL);

    return box->_pref_padding;
}

void ntg_box_set_spacing(ntg_box_t* box, size_t spacing)
{
    assert(box != NULL);

    box->_spacing = spacing;
}

size_t ntg_box_get_spacing(const ntg_box_t* box)
{
    assert(box != NULL);

    return box->_spacing;
}

