#include "core/ntg_simple_stage.h"
#include "core/ntg_simple_scene.h"
#include "core/ntg_stage.h"
#include "ntg.h"
#include "core/ntg_scene.h"
#include "object/ntg_border_container.h"
#include "object/ntg_color_block.h"
#include <stdio.h>
#include <unistd.h>
#include "object/ntg_container.h"
#include "object/ntg_object.h"
#include "object/ntg_box.h"
#include "shared/ntg_log.h"
#include "shared/ntg_vector.h"


struct ntg_object_border __create_border1()
{
    ntg_cell outline = ntg_cell_full('#',
            nt_color_new(255, 255, 255),
            NT_COLOR_DEFAULT,
            NT_STYLE_DEFAULT);

    ntg_cell inside = ntg_cell_default();

    return (struct ntg_object_border) {
        .north_line = outline,
        .east_line = outline,
        .south_line = outline,
        .west_line = outline,

        .north_west_corner = outline,
        .north_east_corner = outline,
        .south_west_corner = outline,
        .south_east_corner = outline,

        .border_inside = inside
    };
}

// // TODO: remove
// #include "object/def/ntg_object_def.h"

void init_gui_fn(ntg_stage* _main_stage, void* data)
{
    ntg_color_block* cb1 = ntg_color_block_new(nt_color_new(255, 0, 0));
    ntg_object_set_pref_size(NTG_OBJECT(cb1), ntg_xy(20, 10));

    ntg_color_block* cb2 = ntg_color_block_new(nt_color_new(0, 255, 0));
    ntg_object_set_pref_size(NTG_OBJECT(cb2), ntg_xy(700, 10));

    ntg_color_block* cb3 = ntg_color_block_new(nt_color_new(0, 0, 255));
    ntg_object_set_pref_size(NTG_OBJECT(cb3), ntg_xy(15, 10));

    ntg_color_block* cb4 = ntg_color_block_new(nt_color_new(50, 50, 50));
    ntg_object_set_pref_size(NTG_OBJECT(cb4), ntg_xy(25, 10));

    ntg_color_block* cb5 = ntg_color_block_new(nt_color_new(100, 100, 100));
    ntg_object_set_pref_size(NTG_OBJECT(cb5), ntg_xy(12, 50));

    ntg_color_block* cb6 = ntg_color_block_new(nt_color_new(150, 150, 150));
    ntg_object_set_pref_size(NTG_OBJECT(cb6), ntg_xy(12, 50));

    ntg_color_block* cb7 = ntg_color_block_new(nt_color_new(200, 200, 200));

    (NTG_OBJECT(cb7))->_border = __create_border1();

    (NTG_OBJECT(cb7))->_border_pref_size.north = 2;
    (NTG_OBJECT(cb7))->_border_pref_size.south = 2;
    (NTG_OBJECT(cb7))->_border_pref_size.west = 2;
    (NTG_OBJECT(cb7))->_border_pref_size.east = 2;

    ntg_object_set_pref_size(NTG_OBJECT(cb7), ntg_xy(0 + 4, 5 + 4));

    ntg_color_block* cb8 = ntg_color_block_new(nt_color_new(255, 255, 0));
    ntg_object_set_pref_size(NTG_OBJECT(cb8), ntg_xy(10, 0));

    ntg_color_block* cb9 = ntg_color_block_new(nt_color_new(255, 0, 255));
    ntg_object_set_pref_size(NTG_OBJECT(cb9), ntg_xy(30, 30));

    ntg_box* box1 = ntg_box_new(NTG_BOX_ORIENTATION_HORIZONTAL,
            NTG_BOX_ALIGNMENT_0,
            NTG_BOX_ALIGNMENT_0);

    (NTG_OBJECT(box1))->_border = __create_border1();

    (NTG_OBJECT(box1))->_border_pref_size.north = 1;
    (NTG_OBJECT(box1))->_border_pref_size.south = 1;
    (NTG_OBJECT(box1))->_border_pref_size.west = 1;
    (NTG_OBJECT(box1))->_border_pref_size.east = 1;

    // struct ntg_box_padding box1_padding = {
    //     .north = 2,
    //     .south = 1,
    //     .east = 0,
    //     .west = 0
    // };
    // ntg_box_set_padding(box1, box1_padding);

    ntg_box_set_bg_color(box1, nt_color_new(255, 255, 255));

    ntg_box_add_child(box1, NTG_OBJECT(cb1));
    ntg_box_add_child(box1, NTG_OBJECT(cb2));
    ntg_box_add_child(box1, NTG_OBJECT(cb3));
    ntg_box_add_child(box1, NTG_OBJECT(cb4));

    ntg_box* box2 = ntg_box_new(NTG_BOX_ORIENTATION_VERTICAL,
            NTG_BOX_ALIGNMENT_0,
            NTG_BOX_ALIGNMENT_0);

    ntg_box_set_bg_color(box2, nt_color_new(255, 255, 255));

    ntg_box_add_child(box2, NTG_OBJECT(cb5));
    ntg_box_add_child(box2, NTG_OBJECT(cb6));

    ntg_border_container* bc = ntg_border_container_new();

    ntg_border_container_set_north(bc, NTG_OBJECT(box1));
    ntg_border_container_set_east(bc, NTG_OBJECT(box2));
    ntg_border_container_set_south(bc, NTG_OBJECT(cb7));
    ntg_border_container_set_west(bc, NTG_OBJECT(cb8));
    ntg_border_container_set_center(bc, NTG_OBJECT(cb9));

    ntg_simple_scene* s1 = ntg_simple_scene_new(ntg_scene_process_key_fn_def);
    ntg_scene_set_root(NTG_SCENE(s1), NTG_OBJECT(bc));
    ntg_stage_set_scene(_main_stage, NTG_SCENE(s1));
    ntg_scene_focus(NTG_SCENE(s1), NTG_OBJECT(box1));

    ntg_loop(_main_stage, NTG_FRAMERATE_DEFAULT);

    ntg_color_block_destroy(cb1);
    ntg_color_block_destroy(cb2);
    ntg_color_block_destroy(cb3);
    ntg_color_block_destroy(cb4);
    ntg_color_block_destroy(cb5);
    ntg_color_block_destroy(cb6);
    ntg_color_block_destroy(cb7);
    ntg_color_block_destroy(cb8);
    ntg_color_block_destroy(cb9);
    ntg_box_destroy(box1);
    ntg_box_destroy(box2);
    ntg_border_container_destroy(bc);

    ntg_simple_scene_destroy(s1);

    return;
}

int main(int argc, char *argv[])
{
    __ntg_init__();

    ntg_simple_stage* main_stage = ntg_simple_stage_new(
            ntg_stage_process_key_fn_def);

    ntg_launch(NTG_STAGE(main_stage), init_gui_fn, NULL);
    // printf("\rMT: Launched NTG thread - STATUS: %d\n", _status);

    // printf("\rMT: Waiting for NTG thread.\n");
    ntg_wait();

    // TODO: shouldnt be after ntg_destroy()
    ntg_simple_stage_destroy(main_stage);

    ntg_destroy();

    // printf("\rMT: Done. Exiting.\n");

    // int n1[] = { 1, 2 };
    // int* n2 = { 1, 2 };
    //
    return 0;
}
