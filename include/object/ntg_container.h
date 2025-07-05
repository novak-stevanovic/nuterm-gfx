#ifndef _NTG_CONTAINER_H_
#define _NTG_CONTAINER_H_

#include "nt_gfx.h"
#include "object/ntg_object.h"

#define NTG_CONTAINER(cnt_ptr) ((ntg_container*)(cnt_ptr))

typedef struct ntg_container ntg_container;

typedef void (*ntg_arrange_children_fn)(ntg_container* container);

struct ntg_container
{
    ntg_object __base;

    nt_color __bg;

    ntg_arrange_children_fn __arrange_children_fn;
};

void __ntg_container_init__(ntg_container* container,
        ntg_nsize_fn nsize_fn, ntg_constrain_fn constrain_fn,
        ntg_measure_fn measure_fn, ntg_arrange_children_fn arrange_fn);

void __ntg_container_deinit__(ntg_container* container);

void _ntg_container_arrange_children(ntg_container* container);

void _ntg_container_set_bg(ntg_container* container, nt_color bg);

#endif // _NTG_CONTAINER_H_

