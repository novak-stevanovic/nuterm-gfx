#ifndef _NTG_SCENE_H_
#define _NTG_SCENE_H_

#include "base/ntg_event.h"
#include "core/ntg_scene_drawing.h"
#include "shared/ntg_xy.h"
#include "nt_event.h"

typedef struct ntg_object ntg_object;
typedef struct ntg_scene ntg_scene;

/* Returns if the scene processed the key event. */
typedef bool (*ntg_scene_process_key_fn)(ntg_scene* scene,
        struct nt_key_event key_event);

typedef void (*ntg_scene_layout_fn)(ntg_scene* scene, struct ntg_xy size);

typedef void (*ntg_scene_on_object_register_fn)(ntg_scene* scene,
        ntg_object* object);

typedef void (*ntg_scene_on_object_unregister_fn)(ntg_scene* scene,
        ntg_object* object);

#define NTG_SCENE(scn_ptr) ((ntg_scene*)(scn_ptr))

/* Abstract */
struct ntg_scene
{
    ntg_object* _root;
    ntg_scene_drawing _drawing;
    ntg_object* _focused;

    ntg_scene_on_object_register_fn __on_object_register_fn;
    ntg_scene_on_object_unregister_fn __on_object_unregister_fn;
    ntg_scene_process_key_fn __process_key_fn;
    ntg_scene_layout_fn __layout_fn;
};

void __ntg_scene_init__(ntg_scene* scene, ntg_scene_layout_fn layout_fn,
        ntg_scene_on_object_register_fn on_object_register_fn,
        ntg_scene_on_object_unregister_fn on_object_unregister_fn);
void __ntg_scene_deinit__(ntg_scene* scene);

void ntg_scene_set_root(ntg_scene* scene, ntg_object* new_root);

void ntg_scene_focus(ntg_scene* scene, ntg_object* object);

/* Should be called by a stage every time the scene should be resized. */
void ntg_scene_layout(ntg_scene* scene, struct ntg_xy size);

void ntg_scene_register_object(ntg_scene* scene, ntg_object* object);
void ntg_scene_unregister_object(ntg_scene* scene, ntg_object* object);

bool ntg_scene_feed_key_event(ntg_scene* scene, struct nt_key_event key_event);

/* By default, the scene will feed the key event to the focus object. */
void _ntg_scene_set_process_key_fn(ntg_scene* scene,
        ntg_scene_process_key_fn process_key_fn);

void ntg_scene_listen(ntg_scene* scene, struct ntg_event_sub sub);

#endif // _NTG_SCENE_H_
