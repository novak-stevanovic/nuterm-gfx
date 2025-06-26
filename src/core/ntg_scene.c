#include "core/ntg_scene.h"
#include <assert.h>
#include <stdlib.h>

#include "core/ntg_scene_drawing.h"
#include "core/ntg_scene_engine.h"
#include "object/ntg_object.h"
#include "shared/ntg_log.h"
#include "shared/ntg_xy.h"

/* -------------------------------------------------------------------------- */

void ntg_scene_layout(ntg_scene* scene, struct ntg_xy size)
{
    ntg_scene_drawing_set_size(&(scene->_drawing), size);
    ntg_scene_engine_layout(scene->_engine);
}

ntg_scene* ntg_scene_new()
{
    ntg_scene* new = (ntg_scene*)malloc(sizeof(struct ntg_scene));

    if(new == NULL) return NULL;

    __ntg_scene_drawing_init__(&new->_drawing);

    new->_engine = ntg_scene_engine_new(new);
    if(new->_engine == NULL)
    {
        __ntg_scene_drawing_deinit__(&new->_drawing);
        free(new);
        return NULL;
    }

    new->_root = NULL;
    new->_focused = NULL;

    return new;
}

void ntg_scene_destroy(ntg_scene* scene)
{
    if(scene == NULL) return;

    __ntg_scene_drawing_deinit__(&scene->_drawing);
    ntg_scene_engine_destroy(scene->_engine);
    
    free(scene);
}

ntg_object* ntg_scene_get_root(const ntg_scene* scene)
{
    return (scene != NULL) ? scene->_root : NULL;
}

void ntg_scene_set_root(ntg_scene* scene, ntg_object* new_root)
{
    if(scene == NULL) return;

    if(scene->_root != NULL)
        _ntg_object_set_scene(scene->_root, NULL);
    
    scene->_root = new_root;
    _ntg_object_set_scene(new_root, scene);
    // TODO
}

const ntg_scene_drawing* ntg_scene_get_drawing(const ntg_scene* scene)
{
    return (scene != NULL) ? &scene->_drawing : NULL;
}

ntg_scene_drawing* _ntg_scene_get_drawing(ntg_scene* scene)
{
    return (scene != NULL) ? &scene->_drawing : NULL;
}

ntg_object* ntg_scene_get_focused(const ntg_scene* scene)
{
    assert(scene != NULL);

    return scene->_focused;
}

void ntg_scene_focus(ntg_scene* scene, ntg_object* object)
{
    assert(scene != NULL);

    if(ntg_object_is_focusable(object))
        scene->_focused = object;
}

bool ntg_scene_feed_key_event(ntg_scene* scene, struct nt_key_event key_event)
{
    bool processed = false;
    if(scene->_root != NULL)
    {
        processed = ntg_object_feed_key_event(scene->_root, key_event);
    }

    return processed;
}

void ntg_scene_feed_resize_event(ntg_scene* scene, struct nt_resize_event resize_event)
{
}
