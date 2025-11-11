#ifndef _NTG_SCENE_COMPOSER_H_
#define _NTG_SCENE_COMPOSER_H_

typedef struct ntg_scene_composer ntg_scene_composer;
typedef struct ntg_scene ntg_scene;
typedef struct ntg_scene_drawing ntg_scene_drawing;

typedef void (*ntg_scene_compose_fn)(ntg_scene_composer* composer);

struct ntg_scene_composer
{
    ntg_scene* __scene;

    ntg_scene_drawing* __drawing;
    ntg_scene_compose_fn __compose_fn;
};

void __ntg_scene_composer_init__(
        ntg_scene_composer* composer,
        ntg_scene* scene,
        ntg_scene_compose_fn compose_fn);
void __ntg_scene_composer_deinit__(ntg_scene_composer* cene_composer);

void ntg_scene_compose(ntg_scene_composer* composer);
const ntg_scene_drawing* ntg_scene_composer_get_drawing(
        const ntg_scene_composer* composer);

#endif // _NTG_SCENE_COMPOSER_H_
