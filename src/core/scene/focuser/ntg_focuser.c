#include "ntg.h"
#include <assert.h>

bool ntg_focuser_dispatch_event(
        ntg_focuser* focuser,
        struct nt_event event,
        ntg_loop_ctx* ctx)
{
    assert(focuser != NULL);

    if(focuser->__dispatch_fn != NULL)
        return focuser->__dispatch_fn(focuser, event, ctx);
    else
        return false;
}

void ntg_focuser_init(
        ntg_focuser* focuser,
        ntg_scene* scene,
        ntg_focuser_dispatch_fn dispatch_fn)
{
    assert(focuser != NULL);
    assert(scene != NULL);

    focuser->_scene = scene;
    focuser->__dispatch_fn = dispatch_fn;
}

void _ntg_focuser_deinit_fn(ntg_entity* entity)
{
    ntg_focuser* focuser = (ntg_focuser*)entity;

    focuser->_scene = NULL;
    focuser->__dispatch_fn = NULL;
}
