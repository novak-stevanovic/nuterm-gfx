#include <stdlib.h>

#include "base/ntg_sap.h"
#include "object/ntg_object.h"
#include "object/def/ntg_box_def.h"
#include "object/shared/ntg_object_vec.h"

static inline size_t _get_psize(ntg_box_orientation_t orientation,
        struct ntg_xy size)
{
    return (orientation == NTG_BOX_ORIENTATION_HORIZONTAL) ? size.x : size.y;
}

static inline size_t _get_ssize(ntg_box_orientation_t orientation,
        struct ntg_xy size)
{
    return (orientation == NTG_BOX_ORIENTATION_HORIZONTAL) ? size.y : size.x;
}

static inline struct ntg_xy _new_size(ntg_box_orientation_t orientation,
        size_t psize, size_t ssize)
{
    return ntg_xy(
        (orientation == NTG_BOX_ORIENTATION_HORIZONTAL) ? psize : ssize,
        (orientation == NTG_BOX_ORIENTATION_HORIZONTAL) ? ssize : psize
    );
}

static void __nsize_fn(ntg_object_t* _box)
{
    const ntg_object_vec_t* children = ntg_object_get_children(_box);

    size_t w = 0, h = 0;
    struct ntg_xy it_size;
    size_t i;

    for(i = 0; i < children->_count; i++)
    {
        it_size = ntg_object_get_nsize(children->_data[i]);

        w += it_size.x;
        h = (it_size.y > h) ? it_size.y : h;
    }

    _ntg_object_set_nsize(_box, ntg_xy(w, h));
}

static void __constrain_fn(ntg_object_t* _box)
{
    const ntg_object_vec_t* children = ntg_object_get_children(_box);

    struct ntg_xy nsize = ntg_object_get_nsize(_box);
    struct ntg_constr constr = ntg_object_get_constr(_box);

    ntg_object_t* it_child;
    struct ntg_xy it_nsize;
    size_t i;
    if((nsize.x <= constr.max_size.x) && (nsize.y <= constr.max_size.y))
    {
        for(i = 0; i < children->_count; i++) {
            it_child = children->_data[i];
            it_nsize = ntg_object_get_nsize(it_child);

            _ntg_object_set_constr(it_child, ntg_constr(it_nsize, it_nsize));
        }
    }
    else if((nsize.x <= constr.max_size.x) && (nsize.y > constr.max_size.y))
    {
        struct ntg_constr base_constr[NTG_BOX_MAX_CHILDREN];

        size_t nsizes[NTG_BOX_MAX_CHILDREN];
        size_t _sizes[NTG_BOX_MAX_CHILDREN];

        for(i = 0; i < children->_count; i++)
        {
            it_child = children->_data[i];
            it_nsize = ntg_object_get_nsize(it_child);
            nsizes[i] = it_nsize.x;

            base_constr[i] = ntg_constr(
                    ntg_xy(it_nsize.x, 0),
                    ntg_xy(it_nsize.x, constr.max_size.y));
        }

        size_t extra = constr.max_size.x - nsize.x;
        ntg_sap_nsize_round_robin(nsizes, _sizes, extra, children->_count);

        for(i = 0; i < children->_count; i++)
        {
            it_child = children->_data[i];

            base_constr[i].max_size.x += _sizes[i];
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
            nsizes[i] = it_nsize.x;

            base_constr[i] = ntg_constr(
                    ntg_xy(0, 0),
                    ntg_xy(0, constr.max_size.y));
        }

        ntg_sap_nsize_round_robin(nsizes, _sizes, constr.max_size.x, children->_count);

        for(i = 0; i < children->_count; i++)
        {
            it_child = children->_data[i];

            base_constr[i].min_size.x += _sizes[i];
            base_constr[i].max_size.x += _sizes[i];

            _ntg_object_set_constr(it_child, base_constr[i]);
        }
    }
}

static void __measure_fn(ntg_object_t* _box)
{
    const ntg_object_vec_t* children = ntg_object_get_children(_box);

    size_t w = 0, h = 0;
    struct ntg_xy it_size;
    size_t i;

    for(i = 0; i < children->_count; i++)
    {
        it_size = ntg_object_get_size(children->_data[i]);

        w += it_size.x;
        h = (it_size.y > h) ? it_size.y : h;
    }

    _ntg_object_set_size(_box, ntg_xy(w, h));
}

static void __arrange_fn(ntg_container_t* box)
{
    ntg_object_t* _box = (ntg_object_t*)box;

    const ntg_object_vec_t* children = ntg_object_get_children(_box);

    size_t w = 0, h = 0;
    struct ntg_xy it_size;
    size_t i;

    for(i = 0; i < children->_count; i++)
    {
        _ntg_object_set_pos(children->_data[i], ntg_xy(w, 0));

        it_size = ntg_object_get_size(children->_data[i]);
        w += it_size.x;
        h = (it_size.y > h) ? it_size.y : h;
    }
}

void __ntg_box_init__(ntg_box_t* box, ntg_box_orientation_t orientation)
{
    __ntg_container_init__((ntg_container_t*)box, __nsize_fn,
            __constrain_fn, __measure_fn, __arrange_fn);

    box->_orientation = orientation;
}

void __ntg_box_deinit__(ntg_box_t* box)
{
    __ntg_container_deinit__((ntg_container_t*)box);
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
    _ntg_container_add_child((ntg_container_t*)box, object);
}
