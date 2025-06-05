#ifndef _NTG_PANE_DEF_H_
#define _NTG_PANE_DEF_H_

#ifndef __NTG_ALLOW_NTG_PANE_DEF__
// #error "Only include this file when requiring the definition."
#endif // __NTG_ALLOW_NTG_PANE_DEF__

#define __NTG_ALLOW_NTG_OBJECT_DEF__
#include "object/def/ntg_object_def.h"
#undef __NTG_ALLOW_NTG_OBJECT_DEF__

struct ntg_pane
{
    ntg_object_t _base;
};

void __ntg_pane_init__(ntg_pane_t* pane, ntg_measure_fn measure_fn,
        ntg_arrange_fn arrange_fn);

void __ntg_pane_deinit__(ntg_pane_t* pane);

#endif // _NTG_PANE_DEF_H_
