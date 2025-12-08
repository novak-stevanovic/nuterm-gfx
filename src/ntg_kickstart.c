#include <stdlib.h>

#include "ntg_kickstart.h"
#include "core/loop/ntg_taskmaster.h"
#include "core/renderer/ntg_def_renderer.h"

struct ntg_kickstart_basic_obj ntg_kickstart_basic(
        ntg_stage* init_stage,
        unsigned int loop_framerate, /* non-zero */
        ntg_def_loop_process_key_fn loop_process_key_fn, /* non-NULL */
        ntg_def_loop_on_resize_fn loop_on_resize_fn,
        ntg_def_loop_on_timeout_fn loop_on_timeout_fn,
        void* loop_data, void* renderer_data)
{
    assert(loop_framerate > 0);
    assert(loop_framerate < 1000);
    // assert(loop_process_key_fn != NULL);

    ntg_def_loop* loop = (ntg_def_loop*)malloc(sizeof(ntg_def_loop));
    ntg_loop* _loop = (ntg_loop*)loop;
    assert(loop != NULL);

    ntg_def_renderer* renderer = (ntg_def_renderer*)malloc(sizeof(ntg_def_renderer));
    ntg_renderer* _renderer = (ntg_renderer*)renderer;
    assert(renderer != NULL);

    ntg_taskmaster* taskmaster = ntg_taskmaster_new();
    
    __ntg_def_loop_init__(
            loop,
            init_stage,
            taskmaster,
            loop_framerate,
            _renderer,
            loop_process_key_fn,
            loop_on_resize_fn,
            loop_on_timeout_fn,
            loop_data);

    __ntg_def_renderer_init__(renderer, _loop, renderer_data);

    return (struct ntg_kickstart_basic_obj) {
        .renderer = renderer,
        ._renderer = _renderer,

        .taskmaster = taskmaster,

        .loop = loop,
        ._loop = _loop
    };
}

void ntg_kickstart_basic_end(struct ntg_kickstart_basic_obj* obj)
{
    assert(obj != NULL);

    __ntg_def_renderer_deinit__(obj->renderer);
    __ntg_def_loop_deinit__(obj->loop);
    ntg_taskmaster_destroy(obj->taskmaster);

    (*obj) = (struct ntg_kickstart_basic_obj) {0};
}

struct ntg_kickstart_scene_obj ntg_kickstart_scene(
        ntg_def_scene_process_key_fn scene_process_key_fn,
        void* scene_data, void* stage_data)
{
    ntg_def_scene* scene = (ntg_def_scene*)malloc(sizeof(ntg_def_scene));
    assert(scene != NULL);

    ntg_def_stage* stage = (ntg_def_stage*)malloc(sizeof(ntg_def_stage));
    assert(stage != NULL);

    __ntg_def_scene_init__(scene, scene_process_key_fn, scene_data);
    __ntg_def_stage_init__(stage, (ntg_scene*)scene, stage_data);

    return (struct ntg_kickstart_scene_obj) {
        .scene = scene,
        ._scene = (ntg_scene*)scene,
        .stage = stage,
        ._stage = (ntg_stage*)stage
    };
}

void ntg_kickstart_scene_end(struct ntg_kickstart_scene_obj* obj)
{
    assert(obj != NULL);

    __ntg_def_scene_deinit__(obj->scene);
    __ntg_def_stage_deinit__(obj->stage);

    (*obj) = (struct ntg_kickstart_scene_obj) {0};
}
