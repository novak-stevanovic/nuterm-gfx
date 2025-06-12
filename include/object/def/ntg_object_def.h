#ifndef _NTG_OBJECT_DEF_H_
#define _NTG_OBJECT_DEF_H_

#include "shared/ntg_xy.h"
#include "object/ntg_object.h"

#ifndef __NTG_ALLOW_NTG_OBJECT_DEF__
// #error "Only include this file when requiring the definition."
#endif // __NTG_ALLOW_NTG_OBJECT_DEF__

/* -------------------------------------------------------------------------- */

struct ntg_object
{
    ntg_object_t* _parent;
    ntg_object_vec_t* _children;
    ntg_object_drawing_t* _drawing;

    struct ntg_xy _pref_size;

    struct ntg_xy _nsize;

    struct ntg_constr _constr;

    struct ntg_xy _size;

    struct ntg_xy _pos;

    ntg_nsize_fn __nsize_fn;
    ntg_constrain_fn __constrain_fn;
    ntg_measure_fn __measure_fn;
    ntg_arrange_fn __arrange_fn;
};

/* ntg_object protected */

void __ntg_object_init__(ntg_object_t* object, ntg_nsize_fn nsize_fn,
        ntg_constrain_fn constrain_fn, ntg_measure_fn measure_fn,
        ntg_arrange_fn arrange_fn, ntg_object_vec_t* children,
        ntg_object_drawing_t* drawing);

void __ntg_object_deinit__(ntg_object_t* object);

void _ntg_object_set_nsize(ntg_object_t* object, struct ntg_xy size);
void _ntg_object_set_constr(ntg_object_t* object, struct ntg_constr constr);
void _ntg_object_set_size(ntg_object_t* object, struct ntg_xy size);
void _ntg_object_set_pos(ntg_object_t* object, struct ntg_xy pos);

ntg_object_drawing_t* _ntg_object_get_drawing_(ntg_object_t* object);

#endif // _NTG_OBJECT_DEF_H_
