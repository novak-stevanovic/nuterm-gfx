#include "core/ntg_scene.h"
#include "core/ntg_scene_drawing.h"
#include "core/ntg_scene_engine.h"
#include "shared/ntg_xy.h"
#include <assert.h>
#include <stdlib.h>

struct ntg_scene
{
    ntg_object_t* root;
    ntg_scene_drawing_t drawing;

    ntg_scene_engine_t* engine;
};

/* -------------------------------------------------------------------------- */

void ntg_scene_layout(ntg_scene_t* scene, struct ntg_xy size)
{
    ntg_scene_drawing_set_size(&(scene->drawing), size);
    ntg_scene_engine_layout(scene->engine);
}

ntg_scene_t* ntg_scene_new()
{
    ntg_scene_t* new = (ntg_scene_t*)malloc(sizeof(struct ntg_scene));

    if(new == NULL) return NULL;

    __ntg_scene_drawing_init__(&new->drawing);
    // if(new->drawing == NULL)
    // {
    //     free(new);
    //     return NULL;
    // }

    new->engine = ntg_scene_engine_new(new);
    if(new->engine == NULL)
    {
        __ntg_scene_drawing_deinit__(&new->drawing);
        free(new);
        return NULL;
    }

    new->root = NULL;

    return new;
}

void ntg_scene_destroy(ntg_scene_t* scene)
{
    if(scene == NULL) return;

    __ntg_scene_drawing_deinit__(&scene->drawing);
    ntg_scene_engine_destroy(scene->engine);
    
    free(scene);
}

ntg_object_t* ntg_scene_get_root(const ntg_scene_t* scene)
{
    return (scene != NULL) ? scene->root : NULL;
}

void ntg_scene_set_root(ntg_scene_t* scene, ntg_object_t* new_root)
{
    if(scene == NULL) return;
    
    scene->root = new_root;
    // TODO
}

const ntg_scene_drawing_t* ntg_scene_get_drawing(const ntg_scene_t* scene)
{
    return (scene != NULL) ? &scene->drawing : NULL;
}

ntg_scene_drawing_t* _ntg_scene_get_drawing(ntg_scene_t* scene)
{
    return (scene != NULL) ? &scene->drawing : NULL;
}
