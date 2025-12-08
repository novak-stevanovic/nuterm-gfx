#ifndef _NTG_KICKSTART_H
#define _NTG_KICKSTART_H

/* Convenience includes & functions */

#include "core/object/ntg_object.h"
#include "core/loop/ntg_def_loop.h"
#include "core/renderer/ntg_def_renderer.h"
#include "core/loop/ntg_taskmaster.h"
#include "core/stage/ntg_def_stage.h"
#include "core/scene/ntg_def_scene.h"

struct ntg_kickstart_basic_obj
{
    ntg_def_loop* loop;
    ntg_loop* _loop;

    ntg_taskmaster* taskmaster;

    ntg_def_renderer* renderer;
    ntg_renderer* _renderer;
};

struct ntg_kickstart_basic_obj ntg_kickstart_basic(
        ntg_stage* init_stage,
        unsigned int loop_framerate, /* non-zero */
        ntg_def_loop_process_key_fn loop_process_key_fn, /* non-NULL */
        ntg_def_loop_on_resize_fn loop_on_resize_fn,
        ntg_def_loop_on_timeout_fn loop_on_timeout_fn,
        void* loop_data, void* renderer_data);
void ntg_kickstart_basic_end(struct ntg_kickstart_basic_obj* obj);

struct ntg_kickstart_scene_obj
{
    ntg_def_stage* stage;
    ntg_stage* _stage;

    ntg_def_scene* scene;
    ntg_scene* _scene;
};

struct ntg_kickstart_scene_obj ntg_kickstart_scene(
        ntg_def_scene_process_key_fn scene_process_key_fn,
        void* scene_data, void* stage_data);
void ntg_kickstart_scene_end(struct ntg_kickstart_scene_obj* obj);

#endif // _NTG_KICKSTART_H
