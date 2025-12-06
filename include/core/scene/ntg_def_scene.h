#ifndef _NTG_DEF_SCENE_H_
#define _NTG_DEF_SCENE_H_

#include "core/scene/ntg_scene.h"

typedef struct ntg_def_scene ntg_def_scene;

typedef bool (*ntg_def_scene_process_key_fn)(
        ntg_def_scene* scene,
        struct nt_key_event key,
        ntg_loop_context* loop_context);

typedef enum ntg_def_scene_process_key_mode
{
    NTG_DEF_SCENE_PROCESS_KEY_FOCUSD_FIRST,
    NTG_DEF_SCENE_PROCESS_KEY_SCENE_FIRST
} ntg_def_scene_process_key_mode;

/* Default scene with implemented layout that allows for wrapping of elements.
 * Layout is always performed in full. */
struct ntg_def_scene
{
    ntg_scene __base;
    
    ntg_def_scene_process_key_mode __process_key_mode;
    ntg_def_scene_process_key_fn __process_key_fn;
};

void __ntg_def_scene_init__(
        ntg_def_scene* scene,
        ntg_def_scene_process_key_fn process_key_fn,
        void* data);
void __ntg_def_scene_deinit__(ntg_def_scene* scene);

void ntg_def_scene_set_process_key_mode(ntg_def_scene* scene,
        ntg_def_scene_process_key_mode mode);
ntg_def_scene_process_key_mode ntg_def_scene_get_process_key_mode(
        const ntg_def_scene* scene);

void __ntg_def_scene_layout_fn(ntg_scene* _scene, struct ntg_xy size);

#endif // _NTG_DEF_SCENE_H_
