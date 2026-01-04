#include "ntg.h"
#include <assert.h>

ntg_single_focuser* ntg_single_focuser_new(ntg_entity_system* system)
{
    struct ntg_entity_init_data entity_data = {
        .type = &NTG_ENTITY_DEF_FOCUSER,
        .deinit_fn = _ntg_single_focuser_deinit_fn,
        .system = system
    };

    ntg_single_focuser* new = (ntg_single_focuser*)ntg_entity_create(entity_data);
    assert(new != NULL);

    return new;
}

void ntg_single_focuser_init(ntg_single_focuser* focuser, ntg_scene* scene)
{
    assert(focuser != NULL);

    ntg_focuser_init((ntg_focuser*)focuser, scene, _ntg_single_focuser_dispatch_fn);

    focuser->_focused = NULL;
}

void ntg_single_focuser_focus(ntg_single_focuser* focuser, ntg_object* object)
{
    assert(focuser != NULL);

    ntg_focuser* _focuser = (ntg_focuser*)focuser;

    if(object != NULL)
    {
        struct ntg_scene_node data = ntg_scene_get_node(_focuser->_scene, object);
        assert(data.exists);
    }

    focuser->_focused = object;
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_single_focuser_deinit_fn(ntg_entity* entity)
{
    ntg_single_focuser* single_focuser = (ntg_single_focuser*)entity;

    single_focuser->_focused = NULL;

    _ntg_focuser_deinit_fn(entity);
}

bool _ntg_single_focuser_dispatch_fn(
        ntg_focuser* focuser,
        struct nt_event event,
        ntg_loop_ctx* ctx)
{
    ntg_single_focuser* single_focuser = (ntg_single_focuser*)focuser;

    if(single_focuser->_focused != NULL)
        return ntg_object_feed_event(single_focuser->_focused, event, ctx);
    else return false;
}
