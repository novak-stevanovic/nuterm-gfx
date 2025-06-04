#include "core/ntg_scene.h"
#include "base/fwd/ntg_cell_fwd.h"
#include "base/ntg_cell.h"
#include "core/ntg_scene_engine.h"
#include "shared/ntg_xy.h"
#include <assert.h>
#include <stdlib.h>

struct ntg_scene_content
{
    ntg_cell_base_grid_t* _content;
    struct ntg_xy _size;
};

void __ntg_scene_content_init__(ntg_scene_content_t* content);
void __ntg_scene_content_deinit__(ntg_scene_content_t* content);

struct ntg_cell_base* ntg_scene_content_at_(ntg_scene_content_t* content,
        struct ntg_xy pos);

void ntg_scene_content_set_size(ntg_scene_content_t* content,
        struct ntg_xy size);

/* -------------------------------------------------------------------------- */

struct ntg_scene
{
    ntg_object_t* root;
    ntg_scene_content_t content;

    ntg_scene_engine_t* engine;
};

void ntg_scene_layout(ntg_scene_t* scene, struct ntg_xy size)
{
    ntg_scene_engine_layout(scene->engine);
}

ntg_scene_t* ntg_scene_new()
{
    ntg_scene_t* new = (ntg_scene_t*)malloc(sizeof(struct ntg_scene));

    if(new == NULL) return NULL;

    __ntg_scene_content_init__(&new->content);
    // if(new->content == NULL)
    // {
    //     free(new);
    //     return NULL;
    // }

    new->engine = ntg_scene_engine_new(new);
    if(new->engine == NULL)
    {
        __ntg_scene_content_deinit__(&new->content);
        free(new);
        return NULL;
    }

    new->root = NULL;

    return new;
}

void ntg_scene_destroy(ntg_scene_t* scene)
{
    if(scene == NULL) return;

    __ntg_scene_content_deinit__(&scene->content);
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

const ntg_scene_content_t* ntg_scene_get_content(const ntg_scene_t* scene)
{
    return (scene != NULL) ? &scene->content : NULL;
}

ntg_scene_content_t* ntg_scene_get_content_(ntg_scene_t* scene)
{
    return (scene != NULL) ? &scene->content : NULL;
}

/* -------------------------------------------------------------------------- */

void __ntg_scene_content_init__(ntg_scene_content_t* content)
{
    if(content == NULL) return;

    content->_content = ntg_cell_base_grid_new(
            NTG_XY(NTG_TERM_MAX_WIDTH, NTG_TERM_MAX_HEIGHT));

    assert(content->_content != NULL);

    content->_size = NTG_XY_SIZE_UNSET;
}

void __ntg_scene_content_deinit__(ntg_scene_content_t* content)
{
    if(content == NULL) return;

    ntg_cell_base_grid_destroy(content->_content);
    content->_size = NTG_XY_SIZE_UNSET;
}

const struct ntg_cell_base* ntg_scene_content_at(const ntg_scene_content_t* content,
        struct ntg_xy pos)
{
    if(content == NULL) return NULL;

    if(!NTG_XY_POS_IN_BOUNDS(pos, content->_size))
        return NULL;

    return ntg_cell_base_grid_at(content->_content, pos);
}

struct ntg_cell_base* ntg_scene_content_at_(ntg_scene_content_t* content,
        struct ntg_xy pos)
{
    if(content == NULL) return NULL;

    if(!NTG_XY_POS_IN_BOUNDS(pos, content->_size))
        return NULL;

    return ntg_cell_base_grid_at_(content->_content, pos);
}

struct ntg_xy ntg_scene_content_get_size(const ntg_scene_content_t* content)
{
    return (content != NULL) ? content->_size : NTG_XY_SIZE_UNSET;
}

void ntg_scene_content_set_size(ntg_scene_content_t* content,
        struct ntg_xy size)
{
    if(content == NULL) return;

    content->_size = size;
}
