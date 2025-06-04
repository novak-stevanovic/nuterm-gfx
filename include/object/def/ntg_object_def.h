#ifndef _NTG_OBJECT_DEF_H_
#define _NTG_OBJECT_DEF_H_

#include "base/fwd/ntg_cell_fwd.h"
#include "shared/ntg_xy.h"
#include "object/fwd/ntg_object_fwd.h"
#include "object/ntg_object_vec.h"

#ifndef __NTG_ALLOW_NTG_OBJECT_DEF__
// #error "Only include this file when requiring the definition."
#endif // __NTG_ALLOW_NTG_OBJECT_DEF__

/* -------------------------------------------------------------------------- */

struct ntg_object
{
    ntg_object_t* _parent;
    ntg_object_vec_t* _children;
    ntg_object_content_t* _content;

    struct ntg_constr _constr;
    struct ntg_xy _size;
    struct ntg_xy _pos;

    ntg_constrain_fn _constrain_fn;
    ntg_measure_fn _measure_fn;
    ntg_arrange_fn _arrange_fn;
};

/* ntg_object protected */

void __ntg_object_init__(ntg_object_t* object, ntg_constrain_fn constrain_fn,
        ntg_measure_fn measure_fn, ntg_arrange_fn arrange_fn,
        ntg_object_vec_t* children, ntg_object_content_t* content);

void __ntg_object_deinit__(ntg_object_t* object);

/* -------------------------------------------------------------------------- */

/* ntg_object protected */

ntg_object_content_t* ntg_object_content_new();
void ntg_object_content_destroy(ntg_object_content_t* content);

ntg_cell_t* ntg_object_content_at_(ntg_object_content_t* content,
        struct ntg_xy pos);

#endif // _NTG_OBJECT_DEF_H_
