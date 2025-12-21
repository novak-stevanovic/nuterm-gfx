#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "core/object/ntg_def_border.h"
#include "core/object/ntg_def_padding.h"
#include "nt_gfx.h"
#include "ntg.h"
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

        ntg_taskmaster* taskmaster = ntg_loop_ctx_get_taskmaster(loop_ctx);

        ntg_log_log("calling ntg_taskmaster_execute_task");
        ntg_taskmaster_execute_task(taskmaster, __task_fn, NULL, __callback_fn, NULL);

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
    ntg_def_scene scene;
    ntg_scene* _scene = (ntg_scene*)&scene;
    _ntg_def_scene_init_(&scene, scene_process_key_fn, NULL, NULL);
    
    ntg_def_stage stage;
    ntg_stage* _stage = (ntg_stage*)&stage;
    _ntg_def_stage_init_(&stage, _scene, NULL);

    struct ntg_kickstart_obj k = ntg_kickstart(_stage, 30, NULL, NULL, NULL);

    ntg_color_block root;
    ntg_object* _root = (ntg_object*)&root;
    _ntg_color_block_init_(&root, nt_color_new_rgb(nt_rgb_new(255, 0, 0)),
            NTG_OBJECT_EVENT_OPS_DEFAULT, NULL, NULL, NULL);

    ntg_scene_set_root(_scene, _root);

    ntg_loop_run(k.loop, NULL);

    _ntg_object_deinit_(_root);

    ntg_kickstart_end(&k);
}

static void gui_fn2(void* data)
{
    ntg_object_container* container = ntg_object_container_new();

    ntg_border_box root;
    ntg_object* _root = (ntg_object*)&root;
    _ntg_border_box_init_(&root, NTG_OBJECT_EVENT_OPS_DEFAULT, NULL, NULL, container);

    ntg_label north;
    ntg_object* _north = (ntg_object*)&north;
    struct ntg_object_event_ops north_event_ops = {
        .on_focus_fn = north_on_focus_fn,
        .on_unfocus_fn = north_on_unfocus_fn,
        .process_key_fn = NULL
    };
    _ntg_label_init_(&north, NTG_ORIENTATION_H, north_event_ops, NULL, NULL, container);
    struct ntg_strv north_text = ntg_strv_from_cstr("Novak");
    ntg_label_set_text(&north, north_text);
    struct nt_gfx north_gfx = {
        .bg = nt_color_new_rgb(nt_rgb_new(255, 0, 0)),
        .fg = nt_color_new_rgb(nt_rgb_new(255, 255, 255)),
        .style = nt_style_new(NT_STYLE_VAL_BOLD, NT_STYLE_VAL_BOLD, NT_STYLE_VAL_BOLD)
    };
    ntg_def_border north_border;
    ntg_def_padding def_padding;
    _ntg_def_border_init_monochrome_(
            &north_border,
            nt_color_new_rgb(nt_rgb_new(255, 255, 255)),
            ntg_padding_width(2, 1, 1, 1),
            container);
    _ntg_def_padding_init_(&def_padding,
            ntg_cell_bg(nt_color_new_rgb(nt_rgb_new(0, 0, 0))),
            ntg_padding_width(1, 1, 2, 1),
            container);
    ntg_label_set_gfx(&north, north_gfx);
    ntg_label_set_primary_alignment(&north, NTG_TEXT_ALIGNMENT_2);
    ntg_object_set_border(_north, (ntg_padding*)&north_border);
    ntg_object_set_padding(_north, (ntg_padding*)&def_padding);

    ntg_box center;
    ntg_object* _center = (ntg_object*)&center;
    _ntg_box_init_(&center, NTG_ORIENTATION_V, NTG_ALIGNMENT_2, NTG_ALIGNMENT_2,
            NTG_OBJECT_EVENT_OPS_DEFAULT, NULL, NULL, container);
    ntg_object_set_min_size(_center, ntg_xy(1000, 1000));

    ntg_color_block center1, center2, center3;
    ntg_object* _center1 = (ntg_object*)&center1;
    ntg_object* _center2 = (ntg_object*)&center2;
    ntg_object* _center3 = (ntg_object*)&center3;
    _ntg_color_block_init_(&center1, nt_color_new_rgb(nt_rgb_new(0, 70, 70)),
            NTG_OBJECT_EVENT_OPS_DEFAULT, NULL, NULL, container);
    _ntg_color_block_init_(&center2, nt_color_new_rgb(nt_rgb_new(0, 140, 140)),
            NTG_OBJECT_EVENT_OPS_DEFAULT, NULL, NULL, container);
    _ntg_color_block_init_(&center3, nt_color_new_rgb(nt_rgb_new(0, 210, 210)),
            NTG_OBJECT_EVENT_OPS_DEFAULT, NULL, NULL, container);
    ntg_box_add_child(&center, _center1);
    ntg_box_add_child(&center, _center2);
    ntg_box_add_child(&center, _center3);

    ntg_box south;
    ntg_object* _south = (ntg_object*)&south;
    _ntg_box_init_(&south, NTG_ORIENTATION_H, NTG_ALIGNMENT_2,
            NTG_ALIGNMENT_2, NTG_OBJECT_EVENT_OPS_DEFAULT, NULL, NULL, container);
    // ntg_object_set_min_size(_south, ntg_xy(1000, 1000));
    ntg_box_set_spacing(&south, 10);
    struct nt_rgb rgb_white = nt_rgb_new(255, 255, 255);
    ntg_object_set_background(_south, ntg_cell_bg(nt_color_new_rgb(rgb_white)));

    ntg_color_block south1;
    ntg_object* _south1 = (ntg_object*)&south1;
    _ntg_color_block_init_(&south1, nt_color_new_rgb(nt_rgb_new(0, 255, 0)),
            NTG_OBJECT_EVENT_OPS_DEFAULT, NULL, NULL, container);
    // ntg_object_set_grow(_south1, ntg_xy(0, 0));

    ntg_color_block south2;
    ntg_object* _south2 = (ntg_object*)&south2;
    _ntg_color_block_init_(&south2, nt_color_new_rgb(nt_rgb_new(0, 0, 255)),
            NTG_OBJECT_EVENT_OPS_DEFAULT, NULL, NULL, container);
    // ntg_object_set_grow(_south2, ntg_xy(0, 0));

    ntg_box_add_child(&south, _south1);
    ntg_box_add_child(&south, _south2);

    ntg_border_box_set_north(&root, _north);
    ntg_border_box_set_center(&root, _center);
    ntg_border_box_set_south(&root, _south);

    ntg_def_scene scene;
    ntg_scene* _scene = (ntg_scene*)&scene;
    _ntg_def_scene_init_(&scene, scene_process_key_fn, NULL, _north);
    
    ntg_def_stage stage;
    ntg_stage* _stage = (ntg_stage*)&stage;
    _ntg_def_stage_init_(&stage, _scene, NULL);

    struct ntg_kickstart_obj k = ntg_kickstart(_stage, 30, NULL, NULL, NULL);

    ntg_scene_set_root(_scene, _root);
    ntg_loop_run(k.loop, NULL);

    _ntg_def_scene_deinit_(&scene);
    _ntg_def_stage_deinit_(&stage);

    ntg_kickstart_end(&k);

    ntg_object_container_destroy(container);
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
