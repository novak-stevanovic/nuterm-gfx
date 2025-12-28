#ifndef _NTG_DEF_FOCUSER_H_
#define _NTG_DEF_FOCUSER_H_

#include "core/scene/focuser/ntg_focuser.h"

typedef struct ntg_def_focuser ntg_def_focuser;
typedef struct ntg_object ntg_object;
typedef struct ntg_entity_system ntg_entity_system;

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_def_focuser
{
    ntg_focuser __base;

    ntg_object* _focused;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_def_focuser* ntg_def_focuser_new(ntg_entity_system* system);
void _ntg_def_focuser_init_(ntg_def_focuser* focuser, ntg_scene* scene);

void ntg_def_focuser_focus(ntg_def_focuser* focuser, ntg_object* object);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_def_focuser_deinit_fn(ntg_entity* entity);

bool _ntg_def_focuser_dispatch_fn(
        ntg_focuser* focuser,
        struct ntg_loop_event event,
        ntg_loop_ctx* ctx);

#endif // _NTG_DEF_FOCUSER_H_
