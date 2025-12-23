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
#include "shared/sarena.h"

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
        ntg_object* scene_object = (ntg_object*)_scene->data;
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

static void gui_fn2(ntg_entity_system* es, void* data)
{
    sarena* arena = sarena_create(100000);
    assert(arena != NULL);

    ntg_entity_group* eg = ntg_entity_group_new(arena);

    ntg_def_stage stage;
    _ntg_def_stage_init_(&stage, eg, es);

    ntg_def_scene scene;
    _ntg_def_scene_init_(&scene, scene_process_key_fn, eg, es);

    struct ntg_kickstart_obj k;
    ntg_kickstart((ntg_stage*)&stage, 30, NULL, eg, es, &k);

    // Root
    ntg_border_box root;
    _ntg_border_box_init_(&root, ntg_border_box_opts_def(), NULL, eg, es);

    // North
    ntg_label north;
    struct nt_gfx north_gfx = {
        .bg = nt_color_new_rgb(nt_rgb_new(255, 0, 0)),
        .fg = nt_color_new_rgb(nt_rgb_new(255, 255, 255)),
        .style = nt_style_new(NT_STYLE_VAL_BOLD, NT_STYLE_VAL_BOLD, NT_STYLE_VAL_BOLD)
    };
    struct ntg_label_opts north_opts = ntg_label_opts_def();
    north_opts.gfx = north_gfx;
    north_opts.palignment = NTG_TEXT_ALIGNMENT_1;
    ntg_def_border north_border;
    ntg_def_padding north_padding;
    _ntg_def_border_init_monochrome_(
            &north_border,
            nt_color_new_rgb(nt_rgb_new(255, 255, 255)),
            ntg_padding_width(2, 1, 1, 1), eg, es);
    _ntg_def_padding_init_(&north_padding,
            ntg_cell_bg(nt_color_new_rgb(nt_rgb_new(0, 0, 0))),
            ntg_padding_width(1, 1, 2, 1), eg, es);
    _ntg_label_init_(&north, north_opts, NULL, eg, es);
    struct ntg_strv north_text = ntg_strv_from_cstr("Novak");
    ntg_label_set_text(&north, north_text);
    ntg_object_set_padding((ntg_object*)&north, (ntg_padding*)&north_padding);
    ntg_object_set_border((ntg_object*)&north, (ntg_padding*)&north_border);

    // Center
    ntg_box center;
    ntg_object* _center = (ntg_object*)&center;
    struct ntg_box_opts center_opts = ntg_box_opts_def();
    center_opts.orientation = NTG_ORIENTATION_V;
    center_opts.palignment = NTG_ALIGNMENT_2;
    center_opts.salignment = NTG_ALIGNMENT_2;
    _ntg_box_init_(&center, center_opts, NULL, eg, es);
    ntg_object_set_min_size(_center, ntg_xy(1000, 1000));

    // Center children
    ntg_color_block center1, center2, center3;
    _ntg_color_block_init_(&center1, nt_color_new_rgb(nt_rgb_new(0, 70, 70)),
            NULL, eg, es);
    _ntg_color_block_init_(&center2, nt_color_new_rgb(nt_rgb_new(0, 140, 140)),
            NULL, eg, es);
    _ntg_color_block_init_(&center3, nt_color_new_rgb(nt_rgb_new(0, 210, 210)),
            NULL, eg, es);
    ntg_box_add_child(&center, (ntg_object*)&center1);
    ntg_box_add_child(&center, (ntg_object*)&center2);
    ntg_box_add_child(&center, (ntg_object*)&center3);

    // South
    ntg_box south;
    struct ntg_box_opts south_opts = ntg_box_opts_def();
    south_opts.orientation = NTG_ORIENTATION_H;
    south_opts.palignment = NTG_ALIGNMENT_2;
    south_opts.salignment = NTG_ALIGNMENT_2;
    south_opts.spacing = 10;
    _ntg_box_init_(&south, south_opts, NULL, eg, es);
    // ntg_object_set_min_size(_south, ntg_xy(1000, 1000));
    struct nt_rgb rgb_white = nt_rgb_new(255, 255, 255);
    ntg_object_set_background((ntg_object*)&south, ntg_cell_bg(nt_color_new_rgb(rgb_white)));

    // South children
    ntg_color_block south1;
    _ntg_color_block_init_(&south1, nt_color_new_rgb(nt_rgb_new(0, 255, 0)),
            NULL, eg, es);
    // ntg_object_set_grow(_south1, ntg_xy(0, 0));
    ntg_color_block south2;
    _ntg_color_block_init_(&south2, nt_color_new_rgb(nt_rgb_new(0, 0, 255)),
            NULL, eg, es);
    // ntg_object_set_grow(_south2, ntg_xy(0, 0));

    // Connect south
    ntg_box_add_child(&south, (ntg_object*)&south1);
    ntg_box_add_child(&south, (ntg_object*)&south2);
    
    // Connect root
    ntg_border_box_set_north(&root, (ntg_object*)&north);
    ntg_border_box_set_center(&root, (ntg_object*)&center);
    ntg_border_box_set_south(&root, (ntg_object*)&south);

    // Run
    ntg_scene_set_root((ntg_scene*)&scene, (ntg_object*)&root);
    ntg_stage_set_scene((ntg_stage*)&stage, (ntg_scene*)&scene);
    ntg_loop_run(&k.loop, NULL);

    ntg_entity_group_destroy(eg);
    sarena_destroy(arena);
}

int main(int argc, char *argv[])
{
    _ntg_init_();

    ntg_launch(gui_fn2, NULL);

    ntg_wait();

    _ntg_deinit_();

    return 0;
}
