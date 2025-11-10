#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include "core/app/ntg_simple_app_renderer.h"
#include "core/object/ntg_color_block.h"
#include "ntg.h"
#include "core/scene/ntg_lscene.h"

#define COUNT 100

static ntg_app_status app_process_key_fn1(
        struct nt_key_event key_event,
        struct ntg_app_loop_context* context,
        void* data)
{
    return NTG_APP_QUIT;
}

static void gui_fn1(void* data)
{
    ntg_color_block root;
    ntg_object* _root = (ntg_object*)&root;
    __ntg_color_block_init__(&root, nt_color_new(255, 0, 0));

    ntg_lscene scene;
    ntg_scene* _scene = (ntg_scene*)&scene;
    __ntg_lscene_init__(&scene, NULL);
    ntg_scene_set_root(_scene, ntg_object_get_drawable_(_root));

    ntg_simple_app_renderer renderer;
    __ntg_simple_app_renderer_init__(&renderer);

    ntg_app_loop(
            (ntg_scene*)&scene,
            NTG_APP_FRAMERATE_DEFAULT,
            (ntg_app_renderer*)&renderer,
            app_process_key_fn1,
            NULL);

    __ntg_simple_app_renderer_deinit__(&renderer);
    __ntg_color_block_deinit__(&root);
    __ntg_lscene_deinit__(&scene);
}

int main(int argc, char *argv[])
{
    __ntg_app_init__();

    ntg_app_launch(gui_fn1, NULL);

    ntg_app_wait();

    __ntg_app_deinit__();

    // size_t space_pool = 1;
    // size_t caps[COUNT];
    // size_t grows[COUNT];
    // size_t _sizes[COUNT];
    //
    // size_t i;
    // for(i = 0; i < COUNT; i++)
    // {
    //     caps[i] = NTG_SIZE_MAX;
    //     grows[i] = 1;
    //     _sizes[i] = 0;
    // }
    //
    // ntg_sap_cap_round_robin(caps, grows, _sizes, space_pool, COUNT);
    //
    // for(i = 0; i < COUNT; i++)
    // {
    //     printf("%ld ", _sizes[i]);
    // }
    // printf("\r\n");

    return 0;
}
