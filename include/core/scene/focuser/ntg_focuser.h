#ifndef _NTG_FOCUSER_H_
#define _NTG_FOCUSER_H_

#include "base/entity/ntg_entity.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_focuser
{
    ntg_entity __base;
    ntg_scene* _scene;
    ntg_focuser_dispatch_fn __dispatch_fn;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

bool ntg_focuser_dispatch_event(
        ntg_focuser* focuser,
        struct nt_event event,
        ntg_loop_ctx* ctx);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_focuser_init_(
        ntg_focuser* focuser,
        ntg_scene* scene,
        ntg_focuser_dispatch_fn dispatch_fn);

void _ntg_focuser_deinit_fn(ntg_entity* entity);

#endif // _NTG_FOCUSER_H_
