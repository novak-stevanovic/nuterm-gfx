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

void loop_process_event_fn(
        ntg_loop* loop,
        ntg_loop_ctx* ctx,
        struct nt_event event)
{
    if(event.type == NT_EVENT_KEY)
    {
        if(nt_key_event_utf32_check(event.key_data, 'q', false))
        {
            ntg_log_log("loop received q");
            ntg_loop_ctx_break(ctx);
        }
    }
}

bool center2_process_key_fn(
        ntg_drawable* drawable,
        struct nt_key_event key,
        struct ntg_process_key_ctx ctx)
{
    // if(nt_key_event_utf32_check(key, 'f', false))
    // {
    //     ntg_log_log("c2 received f");
    //     return true;
    // }
    // if(nt_key_event_utf32_check(key, 'u', false))
    // {
    //     ntg_log_log("c2 received u");
    //     return true;
    // }

    return false;
}

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
        ntg_drawable* drawable,
        struct ntg_focus_ctx ctx)
{
    ntg_label* label = (ntg_label*)ntg_drawable_user_(drawable);

    struct nt_gfx gfx = ntg_label_get_gfx(label);
    gfx.style._value_rgb |= NT_STYLE_VAL_REVERSE;
    gfx.style._value_c8 |= NT_STYLE_VAL_REVERSE;
    gfx.style._value_c256 |= NT_STYLE_VAL_REVERSE;
    ntg_label_set_gfx(label, gfx);
}

void north_on_unfocus_fn(
        ntg_drawable* drawable,
        struct ntg_unfocus_ctx ctx)
{
    ntg_label* label = (ntg_label*)ntg_drawable_user_(drawable);

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

        ntg_taskmaster_channel* taskmaster = ntg_loop_ctx_get_taskmaster(
                loop_ctx);

        ntg_log_log("calling ntg_taskmaster_execute_task");
        ntg_taskmaster_execute_task(taskmaster,
                __task_fn, NULL,
                __callback_fn, NULL);

        return true;
    }
    else if(nt_key_event_utf32_check(key, 'f', false))
    {
        ntg_object* scene_object = (ntg_object*)_scene->_data;
        ntg_scene_focus(_scene, ntg_object_to_drawable_(scene_object));
        ntg_log_log("Focused center middle child");
        return true;
    }
    else if(nt_key_event_utf32_check(key, 'u', false))
    {
        ntg_log_log("Unfocused");
        ntg_scene_focus(_scene, NULL);
        return true;
    }
    else 
    {
        struct ntg_process_key_ctx ctx = {
            .scene = _scene,
            .loop_ctx = loop_ctx
        };
        if(scene->_focused != NULL)
            return scene->_focused->process_key_fn_(scene->_focused, key, ctx);
        else return false;
    }
}

static void gui_fn1(void* data)
{
    struct ntg_kickstart_scene_obj s = ntg_kickstart_scene(NULL, NULL, NULL);
    struct ntg_kickstart_basic_obj b = ntg_kickstart_basic(s._stage, 30,
            loop_process_event_fn, NULL, NULL);

    ntg_color_block root;
    ntg_object* _root = (ntg_object*)&root;
    __ntg_color_block_init__(&root, nt_color_new_rgb(nt_rgb_new(255, 0, 0)),
            NULL, NULL, NULL, NULL);

    ntg_scene_set_root(s._scene, ntg_object_to_drawable_(_root));

    ntg_loop_run(b.loop, NULL);

    __ntg_color_block_deinit__(&root);

    ntg_kickstart_basic_end(&b);
    ntg_kickstart_scene_end(&s);
}

static void gui_fn2(void* data)
{
    ntg_border_box root;
    ntg_object* _root = (ntg_object*)&root;
    __ntg_border_box_init__(&root, NULL, NULL, NULL, NULL);

    ntg_label north;
    ntg_object* _north = (ntg_object*)&north;
    __ntg_label_init__(&north, NTG_ORIENTATION_H, NULL,
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
    __ntg_box_init__(&center, NTG_ORIENTATION_V,
            NTG_ALIGNMENT_2, NTG_ALIGNMENT_2, NULL, NULL, NULL, NULL);
    ntg_object_set_min_size(_center, ntg_xy(1000, 1000));

    ntg_color_block center1, center2, center3;
    ntg_object* _center1 = (ntg_object*)&center1;
    ntg_object* _center2 = (ntg_object*)&center2;
    ntg_object* _center3 = (ntg_object*)&center3;
    __ntg_color_block_init__(&center1, nt_color_new_rgb(nt_rgb_new(0, 70, 70)),
            NULL, NULL, NULL, NULL);
    __ntg_color_block_init__(&center2, nt_color_new_rgb(nt_rgb_new(0, 140, 140)),
            center2_process_key_fn, NULL, NULL, NULL);
    __ntg_color_block_init__(&center3, nt_color_new_rgb(nt_rgb_new(0, 210, 210)),
            NULL, NULL, NULL, NULL);
    ntg_box_add_child(&center, _center1);
    ntg_box_add_child(&center, _center2);
    ntg_box_add_child(&center, _center3);

    ntg_box south;
    ntg_object* _south = (ntg_object*)&south;
    __ntg_box_init__(&south, NTG_ORIENTATION_H, NTG_ALIGNMENT_2,
            NTG_ALIGNMENT_2, NULL, NULL, NULL, NULL);
    // ntg_object_set_min_size(_south, ntg_xy(1000, 1000));
    ntg_box_set_spacing(&south, 10);
    struct nt_rgb rgb_white = nt_rgb_new(255, 255, 255);
    ntg_box_set_background(&south, ntg_cell_bg(nt_color_new_rgb(rgb_white)));

    ntg_color_block south1;
    ntg_object* _south1 = (ntg_object*)&south1;
    __ntg_color_block_init__(&south1, nt_color_new_rgb(nt_rgb_new(0, 255, 0)),
            NULL, NULL, NULL, NULL);
    // ntg_object_set_grow(_south1, ntg_xy(0, 0));

    ntg_color_block south2;
    ntg_object* _south2 = (ntg_object*)&south2;
    __ntg_color_block_init__(&south2, nt_color_new_rgb(nt_rgb_new(0, 0, 255)),
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
            loop_process_event_fn, NULL, NULL);

    ntg_scene_set_root(s._scene, ntg_object_to_drawable_(_root));
    ntg_loop_run(b.loop, NULL);

    __ntg_border_box_deinit__(&root);
    __ntg_label_deinit__(&north);
    __ntg_box_deinit__(&center);
    __ntg_color_block_deinit__(&center1);
    __ntg_color_block_deinit__(&center2);
    __ntg_color_block_deinit__(&center3);
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
