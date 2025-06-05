#ifndef _NTG_OBJECT_DEF_H_
#define _NTG_OBJECT_DEF_H_

#include "base/fwd/ntg_cell_fwd.h"
#include "shared/ntg_xy.h"
#include "object/ntg_object_fwd.h"
#include "object/util/ntg_object_vec.h"

#ifndef __NTG_ALLOW_NTG_OBJECT_DEF__
// #error "Only include this file when requiring the definition."
#endif // __NTG_ALLOW_NTG_OBJECT_DEF__

/* -------------------------------------------------------------------------- */

struct ntg_object
{
    ntg_object_t* _parent;
    ntg_object_vec_t* _children;
    ntg_object_drawing_t* _drawing;

    struct ntg_constr _constr;
    struct ntg_constr _adj_constr;
    struct ntg_xy _size;
    struct ntg_xy _pos;

    ntg_constrain_fn __constrain_fn;
    ntg_measure_fn __measure_fn;
    ntg_arrange_fn __arrange_fn;
};

/* ntg_object protected */

void __ntg_object_init__(ntg_object_t* object, ntg_constrain_fn constrain_fn,
        ntg_measure_fn measure_fn, ntg_arrange_fn arrange_fn);

void __ntg_object_deinit__(ntg_object_t* object);

/* -------------------------------------------------------------------------- */

/* ntg_object protected */

ntg_object_drawing_t* ntg_object_drawing_new();
void ntg_object_drawing_destroy(ntg_object_drawing_t* drawing);

ntg_cell_t* ntg_object_drawing_at_(ntg_object_drawing_t* drawing,
        struct ntg_xy pos);

#endif // _NTG_OBJECT_DEF_H_
