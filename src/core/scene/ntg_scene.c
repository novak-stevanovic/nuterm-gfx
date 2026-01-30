#include <stdlib.h>
#include <assert.h>
#include "ntg.h"
#include "shared/ntg_shared_internal.h"

#define DEBUG 0

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

struct ntg_scene_priv
{
    size_t placeholder;
};

/* -------------------------------------------------------------------------- */
/* LAYOUT */
/* -------------------------------------------------------------------------- */


/* ========================================================================== */
/* HEADER - PUBLIC */
/* ========================================================================== */

static void init_default(ntg_scene* scene)
{
    scene->_stage = NULL;

    scene->_size = ntg_xy(0, 0);

    scene->__on_key_fn = ntg_scene_dispatch_key;
    scene->__on_mouse_fn = ntg_scene_dispatch_mouse;

    scene->data = NULL;
}

void ntg_scene_init(ntg_scene* scene)
{
    assert(scene != NULL);

    scene->__priv = malloc(sizeof(struct ntg_scene_priv));
    assert(scene->__priv);

    init_default(scene);
}

void ntg_scene_deinit(ntg_scene* scene)
{
    if(scene->_stage)
        ntg_stage_set_scene(scene->_stage, NULL);

    init_default(scene);

    if(scene->__priv) free(scene->__priv);
}


bool ntg_scene_layout(ntg_scene* scene, struct ntg_xy size, sarena* arena)
{
    assert(scene != NULL);

}

/* -------------------------------------------------------------------------- */
/* SCENE CTX */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* TREE */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* EVENT */
/* -------------------------------------------------------------------------- */

bool ntg_scene_dispatch_key(ntg_scene* scene, struct nt_key_event key)
{
    assert(scene);

    return false;
}

bool ntg_scene_dispatch_mouse(ntg_scene* scene, struct nt_mouse_event mouse)
{
    assert(scene);

    return false;
}

void ntg_scene_set_on_key_fn(ntg_scene* scene,
        bool (*on_key_fn)(ntg_scene* scene, struct nt_key_event key))
{
    assert(scene);

    scene->__on_key_fn = on_key_fn;
}

bool ntg_scene_on_key(ntg_scene* scene, struct nt_key_event key)
{
    assert(scene);

    if(scene->__on_key_fn)
        return scene->__on_key_fn(scene, key);
    else
        return false;
}

void ntg_scene_set_on_mouse_fn(ntg_scene* scene,
        bool (*on_mouse_fn)(ntg_scene* scene, struct nt_mouse_event mouse))
{
    assert(scene);

    assert(scene);

    scene->__on_mouse_fn = on_mouse_fn;
}

bool ntg_scene_on_mouse(ntg_scene* scene, struct nt_mouse_event mouse)
{
    assert(scene);

    if(scene->__on_mouse_fn)
        return scene->__on_mouse_fn(scene, mouse);
    else
        return false;
}

/* ========================================================================== */
/* HEADER - INTERNAL */
/* ========================================================================== */

void _ntg_scene_set_stage(ntg_scene* scene, ntg_stage* stage)
{
    assert(scene != NULL);

    scene->_stage = stage;
}
