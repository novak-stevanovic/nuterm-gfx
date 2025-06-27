#include "core/ntg_br_stage.h"
#include "core/ntg_sl_scene.h"
#include "core/ntg_stage.h"
#include "ntg.h"
#include "core/ntg_scene.h"
#include "object/ntg_color_block.h"
#include <stdio.h>
#include <unistd.h>
#include "object/ntg_container.h"
#include "object/ntg_object.h"
#include "object/ntg_box.h"
#include "shared/ntg_log.h"
#include "shared/ntg_vector.h"

// // TODO: remove
// #include "object/def/ntg_object_def.h"
#include "object/def/ntg_container_def.h"

ntg_color_block* cb1;
ntg_object* _cb1;

void init_gui_func(void* data)
{
    ntg_stage* _main_stage = ntg_get_stage();

    cb1 = ntg_color_block_new(nt_color_new(255, 0, 90));
    _cb1 = (ntg_object*)cb1;
    ntg_object_set_pref_size(_cb1, ntg_xy(50, 20));

    ntg_color_block* cb2 = ntg_color_block_new(nt_color_new(0, 200, 0));
    ntg_object* _cb2 = (ntg_object*)cb2;
    ntg_object_set_pref_size(_cb2, ntg_xy(100, 10));

    // _ntg_object_scroll_enable(_cb1);
    // __ntg_object_scroll(_cb2, ntg_dxy(0, 1));

    ntg_color_block* cb3 = ntg_color_block_new(nt_color_new(0, 100, 0));
    ntg_object* _cb3 = (ntg_object*)cb3;
    ntg_object_set_pref_size(_cb3, ntg_xy(60, 5));

    ntg_color_block* cb4 = ntg_color_block_new(nt_color_new(0, 100, 200));
    ntg_object* _cb4 = (ntg_object*)cb4;
    ntg_object_set_pref_size(_cb4, ntg_xy(30, 10));

    ntg_box* box1 = ntg_box_new(NTG_BOX_ORIENTATION_HORIZONTAL,
            NTG_BOX_ALIGNMENT_0,
            NTG_BOX_ALIGNMENT_1);

    ntg_object* _box1 = (ntg_object*)box1;
    struct ntg_box_padding box1_padding = (struct ntg_box_padding) {
        .north = 2, .west = 5, .east = 0, .south = 0
    };
    // ntg_box_set_padding(box1, box1_padding);

    ntg_box* box2 = ntg_box_new(NTG_BOX_ORIENTATION_VERTICAL,
            NTG_BOX_ALIGNMENT_0,
            NTG_BOX_ALIGNMENT_1);

    ntg_object* _box2 = (ntg_object*)box2;
    struct ntg_box_padding box2_padding = (struct ntg_box_padding) {
        .north = 6, .west = 10, .east = 5, .south = 3 
    };
    // ntg_box_set_padding(box2, box2_padding);

    ntg_box_add_child(box1, _cb1);
    ntg_box_add_child(box1, _box2);

    ntg_box_add_child(box2, _cb2);
    ntg_box_add_child(box2, _cb3);
    ntg_box_add_child(box2, _cb4);

    _ntg_container_set_bg((ntg_container*)box1, nt_color_new(122, 122, 122));
    _ntg_container_set_bg((ntg_container*)box2, nt_color_new(255, 255, 255));

    ntg_sl_scene* scene1 = ntg_sl_scene_new(ntg_scene_process_key_fn_def);
    ntg_scene* _scene1 = (ntg_scene*)scene1;
    ntg_scene_set_root(_scene1, _box1);
    ntg_stage_set_scene(_main_stage, _scene1);

    ntg_scene_focus(_scene1, _box2);
    ntg_loop(NTG_FRAMERATE_DEFAULT);

    ntg_color_block_destroy(cb1);
    ntg_color_block_destroy(cb2);
    ntg_color_block_destroy(cb3);
    ntg_color_block_destroy(cb4);
    ntg_box_destroy(box1);
    ntg_box_destroy(box2);
    ntg_sl_scene_destroy(scene1);
    return;
}

int main(int argc, char *argv[])
{
    ntg_br_stage* main_stage = ntg_br_stage_new(ntg_stage_process_key_fn_def);
    ntg_initialize((ntg_stage*)main_stage, init_gui_func, NULL);
    
    ntg_launch();
    // printf("\rMT: Launched NTG thread - STATUS: %d\n", _status);

    // printf("\rMT: Waiting for NTG thread.\n");
    ntg_destroy();
    // printf("\rMT: Done. Exiting.\n");

    // int n1[] = { 1, 2 };
    // int* n2 = { 1, 2 };
    //
    return 0;
}
