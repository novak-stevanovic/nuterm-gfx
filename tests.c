#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include "base/event/ntg_event.h"
#include "core/app/ntg_db_app_renderer.h"
#include "core/app/ntg_simple_app_renderer.h"
#include "core/object/ntg_border_box.h"
#include "core/object/ntg_box.h"
#include "core/object/ntg_color_block.h"
#include "core/object/ntg_label.h"
#include "core/stage/ntg_simple_stage.h"
#include "ntg.h"
#include "core/scene/ntg_simple_scene.h"
#include "shared/ntg_log.h"

#define COUNT 100

static ntg_app_status app_process_key_fn1(
        struct nt_key_event key_event,
        struct ntg_app_loop_context* context,
        void* data)
{
    if((key_event.type == NT_KEY_EVENT_UTF32) && (key_event.utf32_data.codepoint == 'q'))
        return NTG_APP_QUIT;
    else
        return NTG_APP_CONTINUE;
}

static void gui_fn1(void* data)
{
    ntg_color_block root;
    ntg_object* _root = (ntg_object*)&root;
    __ntg_color_block_init__(&root, nt_color_new(255, 0, 0));

    ntg_simple_scene scene;
    ntg_scene* _scene = (ntg_scene*)&scene;
    __ntg_simple_scene_init__(&scene, NULL);
    ntg_scene_set_root(_scene, ntg_object_get_drawable_(_root));

    ntg_simple_stage stage;
    ntg_stage* _stage = (ntg_stage*)&stage;
    __ntg_simple_stage_init__(&stage, _scene);

    ntg_db_app_renderer renderer;
    __ntg_db_app_renderer_init__(&renderer);

    ntg_app_loop(
            _stage,
            NTG_APP_FRAMERATE_DEFAULT,
            (ntg_app_renderer*)&renderer,
            app_process_key_fn1,
            NULL);

    __ntg_color_block_deinit__(&root);
    __ntg_simple_scene_deinit__(&scene);
    __ntg_simple_stage_deinit__(&stage);
    __ntg_db_app_renderer_deinit__(&renderer);
}

static void gui_fn2(void* data)
{
    ntg_border_box root;
    ntg_object* _root = (ntg_object*)&root;
    __ntg_border_box_init__(&root);

    ntg_label north;
    ntg_object* _north = (ntg_object*)&north;
    __ntg_label_init__(&north, NTG_ORIENTATION_HORIZONTAL);
    struct ntg_str_view top_text = ntg_str_view_from_cstr("Novak");
    ntg_label_set_text(&north, top_text);
    struct nt_gfx top_gfx = {
        .bg = nt_color_new(255, 0, 0),
        .fg = nt_color_new(255, 255, 255),
        .style = NT_STYLE_BOLD
    };
    ntg_label_set_gfx(&north, top_gfx);
    // ntg_label_set_primary_alignment(&north, NTG_TEXT_ALIGNMENT_2);
    ntg_object_set_max_size(_north, ntg_xy(0, 1));

    ntg_color_block center;
    ntg_object* _center = (ntg_object*)&center;
    __ntg_color_block_init__(&center, nt_color_new(255, 255, 255));
    // TODO: setting min size messes up the size of the label?
    // ntg_object_set_min_size(_center, ntg_xy(1000, 1000));

    ntg_box south;
    ntg_object* _south = (ntg_object*)&south;
    __ntg_box_init__(&south, NTG_ORIENTATION_HORIZONTAL, NTG_ALIGNMENT_2, NTG_ALIGNMENT_2);

    ntg_color_block south1;
    ntg_object* _south1 = (ntg_object*)&south1;
    __ntg_color_block_init__(&south1, nt_color_new(0, 255, 0));
    // ntg_object_set_grow(_south1, ntg_xy(0, 0));

    ntg_color_block south2;
    ntg_object* _south2 = (ntg_object*)&south2;
    __ntg_color_block_init__(&south2, nt_color_new(0, 0, 255));
    // ntg_object_set_grow(_south2, ntg_xy(0, 0));

    ntg_box_add_child(&south, _south1);
    ntg_box_add_child(&south, _south2);

    ntg_border_box_set_north(&root, _north);
    ntg_border_box_set_center(&root, _center);
    ntg_border_box_set_south(&root, _south);

    ntg_simple_scene scene;
    ntg_scene* _scene = (ntg_scene*)&scene;
    __ntg_simple_scene_init__(&scene, NULL);
    ntg_scene_set_root(_scene, ntg_object_get_drawable_(_root));

    ntg_simple_stage stage;
    ntg_stage* _stage = (ntg_stage*)&stage;
    __ntg_simple_stage_init__(&stage, _scene);

    ntg_db_app_renderer renderer;
    __ntg_db_app_renderer_init__(&renderer);

    ntg_app_loop(
            _stage,
            NTG_APP_FRAMERATE_DEFAULT,
            (ntg_app_renderer*)&renderer,
            app_process_key_fn1,
            NULL);

    __ntg_border_box_deinit__(&root);
    __ntg_label_deinit__(&north);
    __ntg_color_block_deinit__(&center);
    __ntg_color_block_deinit__(&south1);
    __ntg_color_block_deinit__(&south2);
    __ntg_box_deinit__(&south);
    __ntg_simple_scene_deinit__(&scene);
    __ntg_simple_stage_deinit__(&stage);
    __ntg_db_app_renderer_deinit__(&renderer);
}

int main(int argc, char *argv[])
{
    __ntg_app_init__();

    ntg_app_launch(gui_fn2, NULL);

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
