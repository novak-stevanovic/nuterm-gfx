#include <stdlib.h>

#include "ntg_kickstart.h"
#include "core/loop/ntg_taskmaster.h"
#include "core/renderer/ntg_def_renderer.h"

struct ntg_kickstart_basic_obj ntg_kickstart_basic(
        ntg_stage* init_stage,
        unsigned int loop_framerate, /* non-zero */
        ntg_loop_process_event_fn loop_process_event_fn,
        void* loop_data, void* renderer_data)
{
    assert(loop_framerate > 0);
    assert(loop_framerate < 1000);
    // assert(loop_process_key_fn != NULL);

    ntg_loop* loop = (ntg_loop*)malloc(sizeof(ntg_loop));
    ntg_loop* _loop = (ntg_loop*)loop;
    assert(loop != NULL);

    ntg_def_renderer* renderer = (ntg_def_renderer*)malloc(sizeof(ntg_def_renderer));
    ntg_renderer* _renderer = (ntg_renderer*)renderer;
    assert(renderer != NULL);

    ntg_taskmaster* taskmaster = ntg_taskmaster_new();
    
    _ntg_loop_init_(
            loop,
            loop_process_event_fn,
            init_stage,
            _renderer,
            taskmaster,
            loop_framerate,
            loop_data);

    _ntg_def_renderer_init_(renderer, _loop, renderer_data);

    return (struct ntg_kickstart_basic_obj) {
        .renderer = renderer,
        ._renderer = _renderer,

        .taskmaster = taskmaster,

        .loop = loop,
    };
}

void ntg_kickstart_basic_end(struct ntg_kickstart_basic_obj* obj)
{
    assert(obj != NULL);

    _ntg_def_renderer_deinit_(obj->renderer);
    _ntg_loop_deinit_(obj->loop);
    ntg_taskmaster_destroy(obj->taskmaster);

    (*obj) = (struct ntg_kickstart_basic_obj) {0};
}

struct ntg_kickstart_scene_obj ntg_kickstart_scene(
        ntg_scene_process_key_fn scene_process_key_fn,
        void* scene_data, void* stage_data)
{
    ntg_def_scene* scene = (ntg_def_scene*)malloc(sizeof(ntg_def_scene));
    assert(scene != NULL);

    ntg_def_stage* stage = (ntg_def_stage*)malloc(sizeof(ntg_def_stage));
    assert(stage != NULL);

    _ntg_def_scene_init_(scene, scene_process_key_fn, scene_data);
    _ntg_def_stage_init_(stage, (ntg_scene*)scene, stage_data);

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

    _ntg_def_scene_deinit_(obj->scene);
    _ntg_def_stage_deinit_(obj->stage);

    (*obj) = (struct ntg_kickstart_scene_obj) {0};
}
