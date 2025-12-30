#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "ntg.h"
#include "core/object/ntg_def_border.h"
#include "core/object/ntg_def_padding.h"
#include "core/scene/focuser/ntg_def_focuser.h"
#include "core/object/ntg_border_box.h"
#include "core/object/ntg_box.h"
#include "core/object/ntg_color_block.h"
#include "core/object/ntg_label.h"
#include "shared/ntg_log.h"
#include "shared/uconv.h"

bool north_event_fn(ntg_object* object, struct ntg_loop_event event, ntg_loop_ctx* ctx)
{
    if(event.event.type == NT_EVENT_KEY)
    {
        char c[] = "Ћ";
        uc_status _status;
        uint32_t cp = uc_utf8_to_utf32_single((uint8_t*)c, strlen(c), 0, &_status);

        if(nt_key_event_utf32_check(event.event.key_data, cp, false))
        {
            ntg_loop_ctx_break(ctx);
            return true;
        }
    }

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

bool loop_event_fn(ntg_loop_ctx* ctx, struct ntg_loop_event event)
{
    if(event.event.type == NT_EVENT_KEY)
    {
        if(nt_key_event_utf32_check(event.event.key_data, 'q', false))
        {
            ntg_loop_ctx_break(ctx);
            return true;
        }
    }

    return false;
}

static void gui_fn(ntg_entity_system* es, ntg_loop* loop, void* data)
{
    // Root
    ntg_border_box* root = ntg_border_box_new(es);
    _ntg_border_box_init_(root);

    // North
    ntg_label* north = ntg_label_new( es);
    _ntg_label_init_(north, NULL);
    struct nt_gfx north_gfx = {
        .bg = nt_color_new_rgb(nt_rgb_new(255, 0, 0)),
        .fg = nt_color_new_rgb(nt_rgb_new(255, 255, 255)),
        .style = nt_style_new(NT_STYLE_VAL_BOLD, NT_STYLE_VAL_BOLD, NT_STYLE_VAL_BOLD)
    };
    struct ntg_strv north_text = ntg_strv_from_cstr("Novak Novak Novak Novak Novak Novak Novak\nEmilija");
    ntg_label_set_text(north, north_text);
    struct ntg_label_opts north_opts = ntg_label_opts_def();
    north_opts.gfx = north_gfx;
    north_opts.palignment = NTG_TEXT_ALIGNMENT_JUSTIFY;
    north_opts.indent = 2;
    north_opts.wrap = NTG_TEXT_WRAP_WORD_WRAP;
    ntg_label_set_opts(north, north_opts);
    ntg_object_set_event_fn((ntg_object*)north, north_event_fn);

    // North border
    ntg_def_border* north_border = ntg_def_border_new(es);
    _ntg_def_border_init_(north_border);
    struct ntg_def_border_style north_border_style = ntg_def_border_style_monochrome(
            nt_color_new_rgb(nt_rgb_new(255, 255, 255)));
    ntg_def_border_set_style(north_border, north_border_style);
    struct ntg_padding_opts north_border_opts = {
        .width = ntg_padding_width(2, 0, 2, 2),
        .mode = NTG_PADDING_ENABLE_ON_NATURAL
    };
    ntg_padding_set_opts((ntg_padding*)north_border, north_border_opts);
    ntg_object_set_border((ntg_object*)north, (ntg_padding*)north_border);

    // North padding
    ntg_def_padding* north_padding = ntg_def_padding_new(es);
    _ntg_def_padding_init_(north_padding);
    struct ntg_padding_opts north_padding_opts = {
        .width = ntg_padding_width(2, 0, 2, 2),
        .mode = NTG_PADDING_ENABLE_ON_NATURAL
    };
    // ntg_object_set_padding((ntg_object*)north, (ntg_padding*)north_padding);
    ntg_padding_set_opts((ntg_padding*)north_padding, north_padding_opts);

    // Center
    ntg_box* center = ntg_box_new(es);
    _ntg_box_init_(center);
    struct ntg_box_opts center_opts = ntg_box_opts_def();
    center_opts.orientation = NTG_ORIENTATION_V;
    center_opts.palignment = NTG_ALIGNMENT_2;
    center_opts.salignment = NTG_ALIGNMENT_2;
    ntg_box_set_opts(center, center_opts);
    ntg_object_set_min_size((ntg_object*)center, ntg_xy(1000, 1000));

    // Center children
    ntg_color_block* center1 = ntg_color_block_new(es);
    _ntg_color_block_init_(center1);
    ntg_color_block_set_color(center1, nt_color_new_rgb(nt_rgb_new(0, 70, 70)));
    ntg_color_block* center2 = ntg_color_block_new(es);
    _ntg_color_block_init_(center2);
    ntg_color_block_set_color(center2, nt_color_new_rgb(nt_rgb_new(0, 140, 140)));
    ntg_color_block* center3 = ntg_color_block_new(es);
    _ntg_color_block_init_(center3);
    ntg_color_block_set_color(center3, nt_color_new_rgb(nt_rgb_new(0, 210, 210)));

    // South
    ntg_box* south = ntg_box_new(es);
    _ntg_box_init_(south);
    struct ntg_box_opts south_opts = ntg_box_opts_def();
    south_opts.orientation = NTG_ORIENTATION_H;
    south_opts.palignment = NTG_ALIGNMENT_2;
    south_opts.salignment = NTG_ALIGNMENT_2;
    south_opts.spacing = 10;
    ntg_box_set_opts(south, south_opts);
    ntg_object_set_min_size((ntg_object*)south, ntg_xy(1000, 1000));
    struct nt_rgb rgb_white = nt_rgb_new(255, 255, 255);
    ntg_object_set_background((ntg_object*)south, ntg_vcell_bg(nt_color_new_rgb(rgb_white)));
    ntg_def_padding* south_padding = ntg_def_padding_new(es);
    _ntg_def_padding_init_(south_padding);
    struct ntg_padding_opts south_padding_opts = {
        .width = ntg_padding_width(2, 0, 2, 2),
        .mode = NTG_PADDING_ENABLE_ALWAYS
    };
    ntg_padding_set_opts((ntg_padding*)south_padding, south_padding_opts);
    ntg_object_set_padding((ntg_object*)south, (ntg_padding*)south_padding);

    // South children
    ntg_color_block* south1 = ntg_color_block_new(es);
    _ntg_color_block_init_(south1);
    ntg_color_block_set_color(south1, nt_color_new_rgb(nt_rgb_new(0, 255, 0)));
    ntg_color_block* south2 = ntg_color_block_new(es);
    _ntg_color_block_init_(south2);
    ntg_color_block_set_color(south2, nt_color_new_rgb(nt_rgb_new(0, 0, 255)));

    // Connect center
    ntg_box_add_child(center, (ntg_object*)center1);
    ntg_box_add_child(center, (ntg_object*)center2);
    ntg_box_add_child(center, (ntg_object*)center3);

    // Connect south
    ntg_box_add_child(south, (ntg_object*)south1);
    ntg_box_add_child(south, (ntg_object*)south2);
    
    // Connect root
    ntg_border_box_set(root, (ntg_object*)north, NTG_BORDER_BOX_NORTH);
    ntg_border_box_set(root, (ntg_object*)center, NTG_BORDER_BOX_CENTER);
    ntg_border_box_set(root, (ntg_object*)south, NTG_BORDER_BOX_SOUTH);

    // Stage
    ntg_def_stage* stage = ntg_def_stage_new(es);
    _ntg_def_stage_init_(stage, loop);

    // Scene
    ntg_def_focuser* focuser = ntg_def_focuser_new(es);
    ntg_def_scene* scene = ntg_def_scene_new(es);
    _ntg_def_scene_init_(scene, (ntg_focuser*)focuser);
    _ntg_def_focuser_init_(focuser, (ntg_scene*)scene);

    // Connect root-scene-stage
    ntg_scene_set_root((ntg_scene*)scene, (ntg_object*)root);

    ntg_stage_set_scene((ntg_stage*)stage, (ntg_scene*)scene);

    // Loop, renderer, taskmaster
    ntg_def_renderer* renderer = ntg_def_renderer_new(es);
    ntg_taskmaster* taskmaster = ntg_taskmaster_new(es);
    _ntg_def_renderer_init_(renderer);
    _ntg_taskmaster_init_(taskmaster);

    // Run
    struct ntg_loop_run_data loop_data = {
        .event_fn = loop_event_fn,
        .event_mode = NTG_LOOP_EVENT_DISPATCH_FIRST,
        .renderer = (ntg_renderer*)renderer,
        .taskmaster = taskmaster,
        .framerate = 30,
        .stage = (ntg_stage*)stage,
        .ctx_data = NULL
    };

    ntg_def_focuser_focus(focuser, (ntg_object*)north);
    // ntg_def_focuser_focus(focuser, NULL);
    ntg_loop_run(loop, loop_data);
}

int main(int argc, char *argv[])
{
    _ntg_init_();

    ntg_launch(gui_fn, NULL);

    ntg_wait();

    _ntg_deinit_();

    return 0;
}
