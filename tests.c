#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include "base/ntg_event.h"
#include "ntg.h"

#include "core/ntg_simple_stage.h"
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

void test_row_alignment()
{
    size_t total_row_len = 10; // CHANGE THIS
    assert(total_row_len > 0);
    char* str = "novak"; // CHANGE THIS
    size_t content_len = strlen(str);
    size_t indent = 6; // CHANGE THIS

    uint32_t* row = (uint32_t*)malloc(sizeof(uint32_t) * total_row_len);
    assert(row != NULL);

    size_t i;
    for(i = 0; i < content_len; i++)
        row[i] = str[i];

    ntg_text_row_apply_alignment_and_indent(row, indent, content_len,
            total_row_len, NTG_TEXT_ALIGNMENT_2);

    char* aligned_str = (char*)malloc(total_row_len);
    assert(aligned_str != NULL);

    for(i = 0; i < total_row_len; i++)
        aligned_str[i] = row[i];

    printf("%.*s\n", (int)total_row_len, aligned_str);

    free(row);
    free(aligned_str);
}


void gui_fn2(ntg_stage* main_stage, void* data)
{
    ntg_border_box root;
    __ntg_border_box_init__(&root);

    ntg_scene scene;
    __ntg_scene_init__(&scene, NTG_OBJECT(&root));

    struct ntg_str_view text;
    text.data = "novak";
    text.len = strlen(text.data);

    ntg_color_block cb1, cb2, cb3, cb4, cb5;
    ntg_label l1;
    __ntg_color_block_init__(&cb1, nt_color_new(255, 0, 0));
    __ntg_color_block_init__(&cb2, nt_color_new(0, 255, 0));
    __ntg_color_block_init__(&cb3, nt_color_new(0, 0, 255));
    __ntg_color_block_init__(&cb4, nt_color_new(50, 50, 50));
    __ntg_label_init__(&l1, NTG_ORIENTATION_HORIZONTAL);
    ntg_label_set_text(&l1, text);
    // __ntg_color_block_init__(&cb5, nt_color_new(150, 150, 150));

    ntg_border_box_set_north(&root, NTG_OBJECT(&cb1));
    ntg_border_box_set_east(&root, NTG_OBJECT(&cb2));
    ntg_border_box_set_south(&root, NTG_OBJECT(&cb3));
    ntg_border_box_set_west(&root, NTG_OBJECT(&cb4));
    ntg_border_box_set_center(&root, NTG_OBJECT(&l1));
    // ntg_border_box_set_center(&root, NTG_OBJECT(&cb5));

    ntg_stage_set_scene(main_stage, &scene);
    ntg_loop(main_stage, 60);

    __ntg_color_block_deinit__(&cb1);
    __ntg_color_block_deinit__(&cb2);
    __ntg_color_block_deinit__(&cb3);
    __ntg_color_block_deinit__(&cb4);
    __ntg_label_deinit__(&l1);
    // __ntg_color_block_deinit__(&cb5);
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

    __ntg_deinit__();

    return 0;
}
