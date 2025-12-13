#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "nt_gfx.h"
#include "ntg.h"
#include "ntg_kickstart.h"
#include "core/object/ntg_border_box.h"
#include "core/object/ntg_box.h"
#include "core/object/ntg_color_block.h"
#include "core/object/ntg_label.h"
#include "shared/ntg_log.h"

#define COUNT 100

void* __task_fn(void* data)
{
    sleep(5);

    return NULL;
}

void __callback_fn(void* data, void* task_result)
{
    ntg_log_log("%p", task_result);
}

void north_on_focus_fn(
        ntg_object* object,
        struct ntg_object_focus_ctx ctx)
{
    ntg_label* label = (ntg_label*)object;

    struct nt_gfx gfx = ntg_label_get_gfx(label);
    gfx.style._value_rgb |= NT_STYLE_VAL_REVERSE;
    gfx.style._value_c8 |= NT_STYLE_VAL_REVERSE;
    gfx.style._value_c256 |= NT_STYLE_VAL_REVERSE;
    ntg_label_set_gfx(label, gfx);
}

void north_on_unfocus_fn(
        ntg_object* object,
        struct ntg_object_unfocus_ctx ctx)
{
    ntg_label* label = (ntg_label*)object;

    struct nt_gfx gfx = ntg_label_get_gfx(label);
    gfx.style._value_rgb ^= NT_STYLE_VAL_REVERSE;
    gfx.style._value_c8 ^= NT_STYLE_VAL_REVERSE;
    gfx.style._value_c256 ^= NT_STYLE_VAL_REVERSE;
    ntg_label_set_gfx(label, gfx);
}

bool scene_process_key_fn(
        ntg_scene* scene,
        struct nt_key_event key,
        ntg_loop_ctx* loop_ctx)
{
    ntg_scene* _scene = (ntg_scene*)scene;

    if(nt_key_event_utf32_check(key, 's', false))
    {
        ntg_log_log("scene received s");
        return true;
    }
    else if(nt_key_event_utf32_check(key, 'w', false))
    {
        ntg_log_log("scene received w");

        ntg_taskmaster_channel taskmaster = ntg_loop_ctx_get_taskmaster(
                loop_ctx);

        ntg_log_log("calling ntg_taskmaster_execute_task");
        ntg_taskmaster_execute_task(&taskmaster,
                __task_fn, NULL,
                __callback_fn, NULL);

        return true;
    }
    else if(nt_key_event_utf32_check(key, 'f', false))
    {
        ntg_object* scene_object = (ntg_object*)_scene->_data;
        ntg_scene_focus(_scene, scene_object);
        ntg_log_log("Focused center middle child");
        return true;
    }
    else if(nt_key_event_utf32_check(key, 'u', false))
    {
        ntg_log_log("Unfocused");
        ntg_scene_focus(_scene, NULL);
        return true;
    }
    else if(nt_key_event_utf32_check(key, 'q', false))
    {
        ntg_loop_ctx_break(loop_ctx);
        return true;
    }
    else 
    {
        struct ntg_object_key_ctx ctx = {
            .scene = _scene,
            .loop_ctx = loop_ctx
        };
        if(scene->_focused != NULL)
            return ntg_object_feed_key(scene->_focused, key, ctx);
        else return false;
    }
}

static void gui_fn1(void* data)
{
    struct ntg_kickstart_scene_obj s = ntg_kickstart_scene(NULL, NULL, NULL);
    struct ntg_kickstart_basic_obj b = ntg_kickstart_basic(s._stage, 30,
            NULL, NULL, NULL);

    ntg_color_block root;
    ntg_object* _root = (ntg_object*)&root;
    _ntg_color_block_init_(&root, nt_color_new_rgb(nt_rgb_new(255, 0, 0)),
            NULL, NULL, NULL, NULL);

    ntg_scene_set_root(s._scene, _root);

    ntg_loop_run(b.loop, NULL);

    _ntg_color_block_deinit_(&root);

    ntg_kickstart_basic_end(&b);
    ntg_kickstart_scene_end(&s);
}

static void gui_fn2(void* data)
{
    ntg_border_box root;
    ntg_object* _root = (ntg_object*)&root;
    _ntg_border_box_init_(&root, NULL, NULL, NULL, NULL);

    ntg_label north;
    ntg_object* _north = (ntg_object*)&north;
    _ntg_label_init_(&north, NTG_ORIENTATION_H, NULL,
            north_on_focus_fn, north_on_unfocus_fn, NULL);
    struct ntg_str_view north_text = ntg_str_view_from_cstr("Novak");
    ntg_label_set_text(&north, north_text);
    struct nt_gfx north_gfx = {
        .bg = nt_color_new_rgb(nt_rgb_new(255, 0, 0)),
        .fg = nt_color_new_rgb(nt_rgb_new(255, 255, 255)),
        .style = nt_style_new(NT_STYLE_VAL_BOLD, NT_STYLE_VAL_BOLD, NT_STYLE_VAL_BOLD)
    };
    ntg_label_set_gfx(&north, north_gfx);
    // ntg_object_set_min_size(_north, ntg_xy(1000, 1000));
    // ntg_label_set_primary_alignment(&north, NTG_TEXT_ALIGNMENT_2);
    // ntg_object_set_max_size(_north, ntg_xy(0, 1));

    ntg_box center;
    ntg_object* _center = (ntg_object*)&center;
    _ntg_box_init_(&center, NTG_ORIENTATION_V,
            NTG_ALIGNMENT_2, NTG_ALIGNMENT_2, NULL, NULL, NULL, NULL);
    ntg_object_set_min_size(_center, ntg_xy(1000, 1000));

    ntg_color_block center1, center2, center3;
    ntg_object* _center1 = (ntg_object*)&center1;
    ntg_object* _center2 = (ntg_object*)&center2;
    ntg_object* _center3 = (ntg_object*)&center3;
    _ntg_color_block_init_(&center1, nt_color_new_rgb(nt_rgb_new(0, 70, 70)),
            NULL, NULL, NULL, NULL);
    _ntg_color_block_init_(&center2, nt_color_new_rgb(nt_rgb_new(0, 140, 140)),
            NULL, NULL, NULL, NULL);
    _ntg_color_block_init_(&center3, nt_color_new_rgb(nt_rgb_new(0, 210, 210)),
            NULL, NULL, NULL, NULL);
    ntg_box_add_child(&center, _center1);
    ntg_box_add_child(&center, _center2);
    ntg_box_add_child(&center, _center3);

    ntg_box south;
    ntg_object* _south = (ntg_object*)&south;
    _ntg_box_init_(&south, NTG_ORIENTATION_H, NTG_ALIGNMENT_2,
            NTG_ALIGNMENT_2, NULL, NULL, NULL, NULL);
    // ntg_object_set_min_size(_south, ntg_xy(1000, 1000));
    ntg_box_set_spacing(&south, 10);
    struct nt_rgb rgb_white = nt_rgb_new(255, 255, 255);
    ntg_object_set_background(_south, ntg_cell_bg(nt_color_new_rgb(rgb_white)));

    ntg_color_block south1;
    ntg_object* _south1 = (ntg_object*)&south1;
    _ntg_color_block_init_(&south1, nt_color_new_rgb(nt_rgb_new(0, 255, 0)),
            NULL, NULL, NULL, NULL);
    // ntg_object_set_grow(_south1, ntg_xy(0, 0));

    ntg_color_block south2;
    ntg_object* _south2 = (ntg_object*)&south2;
    _ntg_color_block_init_(&south2, nt_color_new_rgb(nt_rgb_new(0, 0, 255)),
            NULL, NULL, NULL, NULL);
    // ntg_object_set_grow(_south2, ntg_xy(0, 0));

    ntg_box_add_child(&south, _south1);
    ntg_box_add_child(&south, _south2);

    ntg_border_box_set_north(&root, _north);
    ntg_border_box_set_center(&root, _center);
    ntg_border_box_set_south(&root, _south);

    struct ntg_kickstart_scene_obj s = ntg_kickstart_scene(scene_process_key_fn,
            _north, NULL);
    struct ntg_kickstart_basic_obj b = ntg_kickstart_basic(s._stage, 30,
            NULL, NULL, NULL);

    ntg_scene_set_root(s._scene, _root);
    ntg_loop_run(b.loop, NULL);

    _ntg_border_box_deinit_(&root);
    _ntg_label_deinit_(&north);
    _ntg_box_deinit_(&center);
    _ntg_color_block_deinit_(&center1);
    _ntg_color_block_deinit_(&center2);
    _ntg_color_block_deinit_(&center3);
    _ntg_color_block_deinit_(&south1);
    _ntg_color_block_deinit_(&south2);
    _ntg_box_deinit_(&south);

    ntg_kickstart_scene_end(&s);
    ntg_kickstart_basic_end(&b);
}

int main(int argc, char *argv[])
{
    _ntg_init_();

    ntg_launch(gui_fn2, NULL);

    ntg_wait();

    _ntg_deinit_();

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
