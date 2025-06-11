#include <stdlib.h>

#include "object/ntg_object.h"
#include "object/ntg_object_fwd.h"
#include "object/def/ntg_box_def.h"

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

    _ntg_object_set_nsize(_box, NTG_XY(w, h));
}

static void __constrain_fn(ntg_object_t* _box)
{
    const ntg_object_vec_t* children = ntg_object_get_children(_box);

    struct ntg_xy nsize = ntg_object_get_nsize(_box);
    struct ntg_constr constr = ntg_object_get_constr(_box);

    ntg_object_t* it_child;
    struct ntg_constr it_constr;
    struct ntg_xy it_size;
    size_t i;
    if((nsize.x < constr.max_size.x) && (nsize.y < constr.max_size.y))
    {
        for(i = 0; i < children->_count; i++)
        {
            it_child = children->_data[i];
            it_size = ntg_object_get_nsize(it_child);

            it_constr = NTG_CONSTR(it_size, it_size);
            _ntg_object_set_constr(it_child, it_constr);
        }
    }
    else
    {
        size_t w = constr.max_size.x / children->_count;
        struct ntg_xy size = NTG_XY(w, constr.max_size.y);
        struct ntg_constr child_constr = NTG_CONSTR(size, size);
        for(i = 0; i < children->_count; i++)
        {
            it_child = children->_data[i];
            _ntg_object_set_constr(children->_data[i], child_constr);
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

    _ntg_object_set_size(_box, NTG_XY(w, h));
}

static void __arrange_fn(ntg_object_t* _box)
{
    const ntg_object_vec_t* children = ntg_object_get_children(_box);

    size_t w = 0, h = 0;
    struct ntg_xy it_size;
    size_t i;

    for(i = 0; i < children->_count; i++)
    {
        _ntg_object_set_pos(children->_data[i], NTG_XY(w, 0));

        it_size = ntg_object_get_size(children->_data[i]);
        w += it_size.x;
        h = (it_size.y > h) ? it_size.y : h;
    }
}

void __ntg_box_init__(ntg_box_t* box)
{
    __ntg_container_init__((ntg_container_t*)box, __nsize_fn,
            __constrain_fn, __measure_fn, __arrange_fn);
}

void __ntg_box_deinit__(ntg_box_t* box)
{
    __ntg_container_deinit__((ntg_container_t*)box);
}

ntg_box_t* ntg_box_new()
{
    ntg_box_t* new = (ntg_box_t*)malloc(sizeof(struct ntg_box));
    if(new == NULL) return NULL;

    __ntg_box_init__(new);
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
    _ntg_container_add_child((ntg_container_t*)box, object);
}
