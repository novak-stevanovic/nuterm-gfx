#ifndef _NTG_PANE_H_
#define _NTG_PANE_H_

#include "object/ntg_object.h"

#define NTG_PANE(pane_ptr) ((ntg_pane*)(pane_ptr))

typedef struct ntg_pane ntg_pane;

struct ntg_pane
{
    ntg_object _base;
};

void __ntg_pane_init__(ntg_pane* pane, ntg_calculate_nsize_fn nsize_fn,
        ntg_measure_fn measure_fn, ntg_arrange_fn arrange_fn);

void __ntg_pane_deinit__(ntg_pane* pane);

#endif // _NTG_PANE_H_
