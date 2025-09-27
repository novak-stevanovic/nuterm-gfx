#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include "base/ntg_event.h"
#include "base/ntg_sap.h"
#include "ntg.h"
#include "core/ntg_stage.h"
#include "core/ntg_scene.h"
#include "object/ntg_border_box.h"
#include "object/ntg_label.h"
#include "object/ntg_object.h"
#include "object/ntg_box.h"
#include "shared/ntg_log.h"
#include "shared/ntg_string.h"
#include "shared/ntg_vector.h"
#include "base/ntg_event_types.h"
#include "base/ntg_event_participants.h"
#include "object/ntg_color_block.h"

void gui_fn1(ntg_stage* main_stage, void* data)
{
    ntg_box root;
    __ntg_box_init__(&root, NTG_ORIENTATION_HORIZONTAL,
            NTG_ALIGNMENT_1, NTG_ALIGNMENT_1);

    ntg_scene scene;
    __ntg_scene_init__(&scene, NTG_OBJECT(&root), NULL, NULL, NULL);

    ntg_color_block cb1, cb2, cb3;
    __ntg_color_block_init__(&cb1, nt_color_new(255, 0, 0));
    __ntg_color_block_init__(&cb2, nt_color_new(0, 255, 0));
    __ntg_color_block_init__(&cb3, nt_color_new(0, 0, 255));

    ntg_label l1;
    __ntg_label_init__(&l1, NTG_ORIENTATION_HORIZONTAL);
    struct ntg_str_view l1_text;
    l1_text.data = "test1234567";
    l1_text.len = strlen(l1_text.data);
    ntg_label_set_text(&l1, l1_text);
    struct nt_gfx l1_gfx = {
        .fg = nt_color_new(255, 0, 0),
        .bg = nt_color_new(0, 0, 100),
        .style = NT_STYLE_DEFAULT
    };
    ntg_label_set_gfx(&l1, l1_gfx);
    ntg_label_set_wrap_mode(&l1, NTG_TEXT_WRAP_NOWRAP);
    ntg_label_set_primary_alignment(&l1, NTG_TEXT_ALIGNMENT_JUSTIFY);
    ntg_object_set_grow(NTG_OBJECT(&l1), ntg_xy(3, 0));
    ntg_label_set_indent(&l1, 0);

    ntg_box_add_child(&root, NTG_OBJECT(&cb1));
    ntg_box_add_child(&root, NTG_OBJECT(&cb2));
    ntg_box_add_child(&root, NTG_OBJECT(&cb3));
    ntg_box_add_child(&root, NTG_OBJECT(&l1));

    ntg_stage_set_scene(main_stage, &scene);
    ntg_loop(main_stage, 60);

    __ntg_color_block_deinit__(&cb1);
    __ntg_color_block_deinit__(&cb2);
    __ntg_color_block_deinit__(&cb3);
    __ntg_label_deinit__(&l1);
    __ntg_box_deinit__(&root);
    __ntg_scene_deinit__(&scene);
}

void gui_fn2(ntg_stage* main_stage, void* data)
{
    ntg_border_box root;
    __ntg_border_box_init__(&root);

    ntg_scene scene;
    __ntg_scene_init__(&scene, NTG_OBJECT(&root), NULL, NULL, NULL);

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

    ntg_object_set_grow(NTG_OBJECT(&cb1), ntg_xy(0, 0));
    ntg_object_set_grow(NTG_OBJECT(&cb3), ntg_xy(0, 0));
    ntg_object_set_grow(NTG_OBJECT(&cb5), ntg_xy(0, 0));

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

#define COUNT 100

int main(int argc, char *argv[])
{
    __ntg_init__();

    ntg_stage main_stage;
    __ntg_stage_init__(&main_stage, NULL);

    ntg_launch(&main_stage, gui_fn2, NULL);

    ntg_wait();

    __ntg_stage_deinit__(&main_stage);


    __ntg_deinit__();

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
