#include "core/ntg_scene.h"
#include <assert.h>
#include <stdlib.h>

#include "core/ntg_scene_drawing.h"
#include "core/ntg_scene_engine.h"
#include "object/ntg_object.h"
#include "shared/ntg_log.h"
#include "shared/ntg_xy.h"

struct ntg_scene
{
    ntg_object* root;
    ntg_scene_drawing drawing;

    ntg_scene_engine* engine;

    ntg_object* focused;
};

/* -------------------------------------------------------------------------- */

void ntg_scene_layout(ntg_scene* scene, struct ntg_xy size)
{
    ntg_log_log("Scene layout begin.");
    ntg_scene_drawing_set_size(&(scene->drawing), size);
    ntg_scene_engine_layout(scene->engine);
}

ntg_scene* ntg_scene_new()
{
    ntg_scene* new = (ntg_scene*)malloc(sizeof(struct ntg_scene));

    if(new == NULL) return NULL;

    __ntg_scene_drawing_init__(&new->drawing);

    new->engine = ntg_scene_engine_new(new);
    if(new->engine == NULL)
    {
        __ntg_scene_drawing_deinit__(&new->drawing);
        free(new);
        return NULL;
    }

    new->root = NULL;
    new->focused = NULL;

    return new;
}

void ntg_scene_destroy(ntg_scene* scene)
{
    if(scene == NULL) return;

    __ntg_scene_drawing_deinit__(&scene->drawing);
    ntg_scene_engine_destroy(scene->engine);
    
    free(scene);
}

ntg_object* ntg_scene_get_root(const ntg_scene* scene)
{
    return (scene != NULL) ? scene->root : NULL;
}

void ntg_scene_set_root(ntg_scene* scene, ntg_object* new_root)
{
    if(scene == NULL) return;

    if(scene->root != NULL)
        _ntg_object_set_scene(scene->root, NULL);
    
    scene->root = new_root;
    _ntg_object_set_scene(new_root, scene);
    // TODO
}

const ntg_scene_drawing* ntg_scene_get_drawing(const ntg_scene* scene)
{
    return (scene != NULL) ? &scene->drawing : NULL;
}

ntg_scene_drawing* _ntg_scene_get_drawing(ntg_scene* scene)
{
    return (scene != NULL) ? &scene->drawing : NULL;
}

ntg_object* ntg_scene_get_focused(const ntg_scene* scene)
{
    assert(scene != NULL);

    return scene->focused;
}

void ntg_scene_set_focused(ntg_scene* scene, ntg_object* object)
{
    assert(scene != NULL);

    scene->focused = object;
}
