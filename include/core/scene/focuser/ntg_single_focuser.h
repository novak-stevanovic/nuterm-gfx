#ifndef _NTG_SINGLE_FOCUSER_H_
#define _NTG_SINGLE_FOCUSER_H_

#include "core/scene/focuser/ntg_focuser.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_single_focuser
{
    ntg_focuser __base;

    ntg_object* _focused;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_single_focuser* ntg_single_focuser_new(ntg_entity_system* system);
void ntg_single_focuser_init(ntg_single_focuser* focuser, ntg_scene* scene);

void ntg_single_focuser_focus(ntg_single_focuser* focuser, ntg_object* object);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_single_focuser_deinit_fn(ntg_entity* entity);

bool _ntg_single_focuser_dispatch_fn(
        ntg_focuser* focuser,
        struct nt_event event,
        ntg_loop_ctx* ctx);

#endif // _NTG_SINGLE_FOCUSER_H_
