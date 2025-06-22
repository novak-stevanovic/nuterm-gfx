#ifndef _NTG_PANE_DEF_H_
#define _NTG_PANE_DEF_H_

#include "object/ntg_pane.h"

#ifndef __NTG_ALLOW_NTG_PANE_DEF__
// #error "Only include this file when requiring the definition."
#endif // __NTG_ALLOW_NTG_PANE_DEF__

#define __NTG_ALLOW_NTG_OBJECT_DEF__
#include "object/def/ntg_object_def.h"
#undef __NTG_ALLOW_NTG_OBJECT_DEF__

struct ntg_pane
{
    ntg_object _base;
};

void __ntg_pane_init__(ntg_pane* pane, ntg_nsize_fn nsize_fn,
        ntg_measure_fn measure_fn, ntg_arrange_fn arrange_fn);

void __ntg_pane_deinit__(ntg_pane* pane);

#endif // _NTG_PANE_DEF_H_
