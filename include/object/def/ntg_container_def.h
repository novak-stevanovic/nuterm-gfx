#ifndef _NTG_CONTAINER_DEF_H_
#define _NTG_CONTAINER_DEF_H_

#include "nt_gfx.h"
#include "object/ntg_container.h"

#ifndef __NTG_ALLOW_NTG_CONTAINER_DEF__
// #error "Only include this file when requiring the definition."
#endif // __NTG_ALLOW_NTG_CONTAINER_DEF__

#define __NTG_ALLOW_NTG_OBJECT_DEF__
#include "object/def/ntg_object_def.h"
#undef __NTG_ALLOW_NTG_OBJECT_DEF__

struct ntg_container
{
    ntg_object _base;

    nt_color _bg;

    ntg_arrange_children_fn __arrange_children_fn;
};

void __ntg_container_init__(ntg_container* container,
        ntg_nsize_fn nsize_fn, ntg_constrain_fn constrain_fn,
        ntg_measure_fn measure_fn, ntg_arrange_children_fn arrange_fn);

void __ntg_container_deinit__(ntg_container* container);

void _ntg_container_arrange_children(ntg_container* container);

void _ntg_container_set_bg(ntg_container* container, nt_color bg);

#endif // _NTG_CONTAINER_DEF_H_
