#include "ntg_kickstart.h"
#include "core/renderer/ntg_def_renderer.h"
#include <stdlib.h>

struct ntg_kickstart_basic_obj ntg_kickstart_basic(
        ntg_stage* loop_init_stage,
        unsigned int loop_framerate,
        ntg_dlp_process_key_fn loop_process_key_fn,
        void* loop_process_key_fn_data)
{
    assert(loop_init_stage != NULL);
    assert(loop_framerate > 0);
    assert(loop_framerate < 1000);
    assert(loop_process_key_fn != NULL);

    ntg_def_loop_provider* lp = (ntg_def_loop_provider*)malloc(sizeof(ntg_def_loop_provider));
    assert(lp != NULL);

    ntg_db_renderer* renderer = (ntg_db_renderer*)malloc(sizeof(ntg_db_renderer));
    assert(renderer != NULL);

    __ntg_def_loop_provider_init__(
            lp,
            loop_init_stage,
            loop_framerate,
            (ntg_renderer*)renderer,
            loop_process_key_fn,
            loop_process_key_fn_data);

    ntg_listenable* lp_listenable = ntg_def_loop_provider_get_listenable(lp);

    __ntg_db_renderer_init__(renderer, lp_listenable);

    return (struct ntg_kickstart_basic_obj) {
        .renderer = renderer,
        ._renderer = (ntg_renderer*)renderer,

        .loop_provider = lp,
        ._loop_provider = (ntg_loop_provider*)lp
    };
}

void ntg_kickstart_basic_end(struct ntg_kickstart_basic_obj* obj)
{
    assert(obj != NULL);

    __ntg_db_renderer_deinit__(obj->renderer);
    __ntg_def_loop_provider_deinit__(obj->loop_provider);

    (*obj) = (struct ntg_kickstart_basic_obj) {0};
}

struct ntg_kickstart_scene_obj ntg_kickstart_scene(
        ntg_scene_process_key_fn scene_process_key_fn)
{
    ntg_def_scene* scene = (ntg_def_scene*)malloc(sizeof(ntg_def_scene));
    assert(scene != NULL);

    ntg_def_stage* stage = (ntg_def_stage*)malloc(sizeof(ntg_def_stage));

    __ntg_def_scene_init__(scene, scene_process_key_fn);
    __ntg_def_stage_init__(stage, (ntg_scene*)scene);

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
