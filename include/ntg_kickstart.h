#ifndef _NTG_KICKSTART_H
#define _NTG_KICKSTART_H

/* Convenience includes & functions */

#include "core/object/ntg_object.h"
#include "core/loop/ntg_def_loop.h"
#include "core/renderer/ntg_db_renderer.h"
#include "core/stage/ntg_def_stage.h"
#include "core/scene/ntg_def_scene.h"

struct ntg_kickstart_basic_obj
{
    ntg_def_loop* loop;
    ntg_loop* _loop;

    ntg_db_renderer* renderer;
    ntg_renderer* _renderer;
};

struct ntg_kickstart_basic_obj ntg_kickstart_basic(
        ntg_stage* loop_init_stage, /* non-NULL */
        unsigned int loop_framerate, /* non-zero */
        ntg_def_loop_process_key_fn loop_process_key_fn, /* non-NULL */
        void* loop_process_key_fn_data);
void ntg_kickstart_basic_end(struct ntg_kickstart_basic_obj* obj);

struct ntg_kickstart_scene_obj
{
    ntg_def_stage* stage;
    ntg_stage* _stage;

    ntg_def_scene* scene;
    ntg_scene* _scene;
};

struct ntg_kickstart_scene_obj ntg_kickstart_scene(
        ntg_scene_process_key_fn scene_process_key_fn);
void ntg_kickstart_scene_end(struct ntg_kickstart_scene_obj* obj);

#endif // _NTG_KICKSTART_H
