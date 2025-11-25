#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include "ntg.h"
#include "ntg_kickstart.h"
#include "core/object/ntg_border_box.h"
#include "core/object/ntg_box.h"
#include "core/object/ntg_color_block.h"
#include "core/object/ntg_label.h"
#include "core/object/shared/ntg_object_fx.h"
#include "shared/ntg_log.h"

#define COUNT 100

bool def_loop_process_key_fn(
        ntg_loop_context* context,
        struct nt_key_event key)
{
    if(nt_key_event_utf32_check(key, 'q', false))
    {
        ntg_log_log("loop received q");
        ntg_loop_context_exit(context);
        return true;
    }
    return false;
}

bool scene_process_key_fn(
        ntg_scene* scene,
        struct nt_key_event key)
{
    if(nt_key_event_utf32_check(key, 'q', false))
    {
        ntg_log_log("scene received q");
        return false;
    }
    return false;
}

static void gui_fn1(void* data)
{
    struct ntg_kickstart_scene_obj s = ntg_kickstart_scene(NULL, NULL, NULL);
    struct ntg_kickstart_basic_obj b = ntg_kickstart_basic(s._stage, 60, NULL,
            NULL, NULL, NULL, NULL);

    ntg_color_block root;
    ntg_object* _root = (ntg_object*)&root;
    __ntg_color_block_init__(&root, nt_color_new_rgb(nt_rgb_new(255, 0, 0)),
            NULL, NULL, NULL);

    ntg_scene_set_root(s._scene, ntg_object_get_drawable_(_root));

    ntg_loop_run(b._loop, NULL);

    __ntg_color_block_deinit__(&root);

    ntg_kickstart_basic_end(&b);
    ntg_kickstart_scene_end(&s);
}

static void gui_fn2(void* data)
{
    ntg_border_box root;
    ntg_object* _root = (ntg_object*)&root;
    __ntg_border_box_init__(&root, NULL, NULL, NULL);

    ntg_label north;
    ntg_object* _north = (ntg_object*)&north;
    __ntg_label_init__(&north, NTG_ORIENTATION_HORIZONTAL, NULL, NULL, NULL);
    struct ntg_str_view top_text = ntg_str_view_from_cstr("Novak");
    ntg_label_set_text(&north, top_text);
    struct nt_gfx top_gfx = {
        .bg = nt_color_new_rgb(nt_rgb_new(255, 0, 0)),
        .fg = nt_color_new_rgb(nt_rgb_new(255, 255, 255)),
        .style = nt_style_new(NT_STYLE_VAL_BOLD, NT_STYLE_VAL_BOLD, NT_STYLE_VAL_BOLD)
    };
    ntg_label_set_gfx(&north, top_gfx);
    // ntg_label_set_primary_alignment(&north, NTG_TEXT_ALIGNMENT_2);
    // ntg_object_set_max_size(_north, ntg_xy(0, 1));

    ntg_color_block center;
    ntg_object* _center = (ntg_object*)&center;
    __ntg_color_block_init__(&center, nt_color_new_rgb(nt_rgb_new(255, 255, 255)),
                NULL, NULL, NULL);
    ntg_object_set_min_size(_center, ntg_xy(1000, 1000));

    ntg_box south;
    ntg_object* _south = (ntg_object*)&south;
    __ntg_box_init__(&south, NTG_ORIENTATION_HORIZONTAL, NTG_ALIGNMENT_2,
            NTG_ALIGNMENT_2, NULL, NULL, NULL);

    ntg_color_block south1;
    ntg_object* _south1 = (ntg_object*)&south1;
    __ntg_color_block_init__(&south1, nt_color_new_rgb(nt_rgb_new(0, 255, 0)),
            NULL, NULL, NULL);
    // ntg_object_set_grow(_south1, ntg_xy(0, 0));

    ntg_color_block south2;
    ntg_object* _south2 = (ntg_object*)&south2;
    __ntg_color_block_init__(&south2, nt_color_new_rgb(nt_rgb_new(0, 0, 255)),
            NULL, NULL, NULL);
    // ntg_object_set_grow(_south2, ntg_xy(0, 0));

    ntg_box_add_child(&south, _south1);
    ntg_box_add_child(&south, _south2);

    ntg_border_box_set_north(&root, _north);
    ntg_border_box_set_center(&root, _center);
    ntg_border_box_set_south(&root, _south);

    struct ntg_kickstart_scene_obj s = ntg_kickstart_scene(scene_process_key_fn,
            NULL, NULL);
    struct ntg_kickstart_basic_obj b = ntg_kickstart_basic(s._stage, 60,
            def_loop_process_key_fn, NULL, NULL, NULL, NULL);

    ntg_scene_set_root(s._scene, ntg_object_get_drawable_(_root));
    ntg_loop_run(b._loop, NULL);

    __ntg_border_box_deinit__(&root);
    __ntg_label_deinit__(&north);
    __ntg_color_block_deinit__(&center);
    __ntg_color_block_deinit__(&south1);
    __ntg_color_block_deinit__(&south2);
    __ntg_box_deinit__(&south);

    ntg_kickstart_scene_end(&s);
    ntg_kickstart_basic_end(&b);
}

int main(int argc, char *argv[])
{
    __ntg_init__();

    ntg_launch(gui_fn2, NULL);

    ntg_wait();

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
