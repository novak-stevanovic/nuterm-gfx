#include <stdio.h>
#include <unistd.h>
#include "base/ntg_event.h"
#include "ntg.h"

#include "core/ntg_simple_stage.h"
#include "core/ntg_stage.h"
#include "core/ntg_scene.h"
#include "object/ntg_border_box.h"
#include "object/ntg_object.h"
#include "object/ntg_box.h"
#include "shared/ntg_log.h"
#include "shared/ntg_vector.h"
#include "base/ntg_event_types.h"
#include "base/ntg_event_participants.h"
#include "object/ntg_color_block.h"

void gui_fn1(ntg_stage* main_stage, void* data)
{
    ntg_box root;
    __ntg_box_init__(&root, NTG_ORIENTATION_VERTICAL,
            NTG_ALIGNMENT_2, NTG_ALIGNMENT_2);

    ntg_scene scene;
    __ntg_scene_init__(&scene, NTG_OBJECT(&root));

    ntg_color_block cb1, cb2, cb3;
    __ntg_color_block_init__(&cb1, nt_color_new(255, 0, 0));
    __ntg_color_block_init__(&cb2, nt_color_new(0, 255, 0));
    __ntg_color_block_init__(&cb3, nt_color_new(0, 0, 255));

    ntg_object_set_min_size(NTG_OBJECT(&cb1), ntg_xy(100, 10));

    ntg_box_add_child(&root, NTG_OBJECT(&cb1));
    ntg_box_add_child(&root, NTG_OBJECT(&cb2));
    ntg_box_add_child(&root, NTG_OBJECT(&cb3));

    ntg_stage_set_scene(main_stage, &scene);
    ntg_loop(main_stage, 60);

    __ntg_color_block_deinit__(&cb1);
    __ntg_color_block_deinit__(&cb2);
    __ntg_color_block_deinit__(&cb3);
    __ntg_box_deinit__(&root);
    __ntg_scene_deinit__(&scene);
}

void gui_fn2(ntg_stage* main_stage, void* data)
{
    ntg_border_box root;
    __ntg_border_box_init__(&root);

    ntg_scene scene;
    __ntg_scene_init__(&scene, NTG_OBJECT(&root));

    ntg_color_block cb1, cb2, cb3, cb4, cb5;
    __ntg_color_block_init__(&cb1, nt_color_new(255, 0, 0));
    __ntg_color_block_init__(&cb2, nt_color_new(0, 255, 0));
    __ntg_color_block_init__(&cb3, nt_color_new(0, 0, 255));
    __ntg_color_block_init__(&cb4, nt_color_new(50, 50, 50));
    __ntg_color_block_init__(&cb5, nt_color_new(150, 150, 150));

    ntg_border_box_set_north(&root, NTG_OBJECT(&cb1));
    ntg_border_box_set_east(&root, NTG_OBJECT(&cb2));
    ntg_border_box_set_south(&root, NTG_OBJECT(&cb3));
    ntg_border_box_set_west(&root, NTG_OBJECT(&cb4));
    ntg_border_box_set_center(&root, NTG_OBJECT(&cb5));

    ntg_stage_set_scene(main_stage, &scene);
    ntg_loop(main_stage, 60);

    __ntg_color_block_deinit__(&cb1);
    __ntg_color_block_deinit__(&cb2);
    __ntg_color_block_deinit__(&cb3);
    __ntg_color_block_deinit__(&cb4);
    __ntg_color_block_deinit__(&cb5);
    __ntg_border_box_deinit__(&root);
    __ntg_scene_deinit__(&scene);
}

int main(int argc, char *argv[])
{
    __ntg_init__();

    ntg_simple_stage* main_stage = ntg_simple_stage_new();

    ntg_launch(NTG_STAGE(main_stage), gui_fn2, NULL);

    ntg_wait();

    ntg_simple_stage_destroy(main_stage);

    ntg_destroy();

    return 0;
}
