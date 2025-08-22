#include <stdio.h>
#include <unistd.h>
#include "base/ntg_event.h"
#include "ntg.h"

#include "core/ntg_simple_stage.h"
#include "core/ntg_stage.h"
#include "core/ntg_scene.h"
#include "object/ntg_object.h"
#include "object/ntg_box.h"
#include "shared/ntg_log.h"
#include "shared/ntg_vector.h"
#include "base/ntg_event_types.h"
#include "base/ntg_event_participants.h"
#include "object/ntg_color_block.h"

void gui_fn1(ntg_stage* main_stage, void* data)
{
    ntg_scene scene;
    __ntg_scene_init__(&scene);

    ntg_box root;
    __ntg_box_init__(&root, NTG_ORIENTATION_VERTICAL,
            NTG_ALIGNMENT_1, NTG_ALIGNMENT_1);

    ntg_color_block cb1, cb2, cb3;
    __ntg_color_block_init__(&cb1, nt_color_new(255, 0, 0));
    __ntg_color_block_init__(&cb2, nt_color_new(0, 255, 0));
    __ntg_color_block_init__(&cb3, nt_color_new(0, 0, 255));

    ntg_box_add_child(&root, NTG_OBJECT(&cb1));
    ntg_box_add_child(&root, NTG_OBJECT(&cb2));
    ntg_box_add_child(&root, NTG_OBJECT(&cb3));

    ntg_scene_set_root(&scene, NTG_OBJECT(&root));
    ntg_stage_set_scene(main_stage, &scene);
    ntg_loop(main_stage, 60);

    __ntg_scene_deinit__(&scene);
    __ntg_box_deinit__(&root);
}

int main(int argc, char *argv[])
{
    __ntg_init__();

    ntg_simple_stage* main_stage = ntg_simple_stage_new();

    ntg_launch(NTG_STAGE(main_stage), gui_fn1, NULL);

    ntg_wait();

    ntg_simple_stage_destroy(main_stage);

    ntg_destroy();

    return 0;
}
