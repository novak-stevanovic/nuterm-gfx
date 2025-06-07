#include "object/def/ntg_box_def.h"
#include "object/ntg_box.h"

#include "object/ntg_object.h"
#include "object/ntg_object_fwd.h"

static void __nsize_fn(ntg_object_t* _box)
{
    ntg_object_vec_t* children = _box->_children;
    size_t count = ntg_object_vec_size(children);

    size_t width = 0, height = 0;

    size_t i;
    ntg_object_t* it_obj;
    struct ntg_xy it_nsize;
    for(i = 0; i < count; i++)
    {
        it_obj = ntg_object_vec_at(children, i);
        it_nsize = ntg_object_get_nsize(it_obj);

        width += it_nsize.x;
        height = (height > it_nsize.y) ? height : it_nsize.y;
    }

    ntg_object_set_nsize(_box, NTG_XY(width, height));
}

static void __constrain_fn(ntg_object_t* _box)
{
    ntg_object_vec_t* children = _box->_children;
    size_t count = ntg_object_vec_size(children);
    struct ntg_xy nsize = ntg_object_get_nsize(_box);
    struct ntg_constr constr = ntg_object_get_constr(_box);

    if((nsize.x > constr.max_size.x) || (nsize.y > constr.max_size.y))
    {
    }
    else
    {
        size_t i;
        ntg_object_t* it_obj;
        struct ntg_xy it_nsize;
        for(i = 0; i < count; i++)
        {
            it_obj = ntg_object_vec_at(children, i);
            it_nsize = ntg_object_get_nsize(it_obj);
            ntg_object_set_constr(it_obj, NTG_CONSTR(it_nsize, it_nsize));
        }
    }

}

static void __measure_fn(ntg_object_t* _box)
{
}

static void __arrange_fn(ntg_object_t* _box)
{
}

void __ntg_box_init__(ntg_box_t* box)
{
    __ntg_container_init__((ntg_container_t*)box, __nsize_fn, __constrain_fn,
            __measure_fn, __arrange_fn);
}

void __ntg_box_deinit__(ntg_box_t* box)
{
    __ntg_container_deinit__((ntg_container_t*)box);
}

