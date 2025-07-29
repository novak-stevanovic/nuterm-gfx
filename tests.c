#include <stdio.h>
#include <unistd.h>
#include "base/ntg_event.h"
#include "ntg.h"

#include "core/ntg_simple_stage.h"
#include "core/ntg_simple_scene.h"
#include "core/ntg_stage.h"
#include "core/ntg_scene.h"
#include "object/ntg_border_container.h"
#include "object/ntg_color_block.h"
#include "object/ntg_container.h"
#include "object/ntg_object.h"
#include "object/ntg_box.h"
#include "object/ntg_prog_bar.h"
#include "shared/ntg_log.h"
#include "shared/ntg_vector.h"
#include "base/ntg_event_types.h"
#include "base/ntg_event_participants.h"

static void __root_change_handler(void* subscriber, ntg_event* event)
{
    if(event->_type == NTG_ETYPE_SCENE_ROOT_CHANGE)
    {
        struct ntg_object_change data = *(struct ntg_object_change*)event->_data;
        ntg_log_log("SUBSCRIBER RECEIVED EVENT: %p | CHANGED ROOT FOR SCENE: %p | OLD: %p | NEW: %p",
                subscriber, event->_source, data.old, data.new);
    }
}

static void __scene_layout_handler(void* subscriber, ntg_event* event)
{
    if(event->_type == NTG_ETYPE_SCENE_LAYOUT)
    {
        ntg_log_log("SUBSCRIBER RECEIVED EVENT: %p | SCENE LAYOUT", subscriber, event->_source);
    }
}

static void __stage_render_handler(void* subscriber, ntg_event* event)
{
    if(event->_type == NTG_ETYPE_STAGE_RENDER)
    {
        ntg_log_log("SUBSCRIBER RECEIVED EVENT: %p | STAGE RENDER", subscriber, event->_source);
    }
}

struct ntg_border_style __create_border1()
{
    ntg_cell outline = ntg_cell_full('#',
            nt_color_new(255, 0, 90),
            NT_COLOR_DEFAULT,
            NT_STYLE_DEFAULT);

    ntg_cell inside = ntg_cell_default();

    return (struct ntg_border_style) {
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

void gui_fn1(ntg_stage* _main_stage, void* data)
{
    ntg_cell completed = ntg_cell_bg(nt_color_new(0, 100, 0));
    ntg_cell threshold = ntg_cell_bg(nt_color_new(0, 200, 0));
    ntg_cell uncompleted = ntg_cell_bg(nt_color_new(100, 0, 0));
    ntg_prog_bar* pb1 = ntg_prog_bar_new(NTG_ORIENTATION_HORIZONTAL, 100,
            completed, uncompleted, threshold);
    ntg_object_set_pref_size(NTG_OBJECT(pb1), ntg_xy(100, 5));

    ntg_color_block* cb2 = ntg_color_block_new(nt_color_new(0, 255, 0));
    ntg_object_set_pref_size(NTG_OBJECT(cb2), ntg_xy(7, 10));

    ntg_color_block* cb3 = ntg_color_block_new(nt_color_new(0, 0, 255));
    ntg_object_set_pref_size(NTG_OBJECT(cb3), ntg_xy(15, 10));

    ntg_color_block* cb4 = ntg_color_block_new(nt_color_new(50, 50, 50));
    ntg_object_set_pref_size(NTG_OBJECT(cb4), ntg_xy(25, 10));

    ntg_color_block* cb5 = ntg_color_block_new(nt_color_new(100, 100, 100));
    ntg_object_set_pref_size(NTG_OBJECT(cb5), ntg_xy(12, 50));

    ntg_color_block* cb6 = ntg_color_block_new(nt_color_new(150, 150, 150));
    ntg_object_set_pref_size(NTG_OBJECT(cb6), ntg_xy(12, 50));

    ntg_color_block* cb7 = ntg_color_block_new(nt_color_new(200, 200, 200));

    ntg_color_block* cb8 = ntg_color_block_new(nt_color_new(255, 255, 0));
    ntg_object_set_pref_size(NTG_OBJECT(cb8), ntg_xy(55, 19));

    ntg_color_block* cb9 = ntg_color_block_new(nt_color_new(255, 0, 255));
    ntg_object_set_pref_size(NTG_OBJECT(cb9), ntg_xy(5500, 5000));

    ntg_box* box1 = ntg_box_new(NTG_ORIENTATION_HORIZONTAL,
            NTG_BOX_ALIGNMENT_2,
            NTG_BOX_ALIGNMENT_2);

    struct ntg_border_size border_size1 = {
        .north = 1,
        .south = 1,
        .west = 1,
        .east = 1
    };
    ntg_object_set_border_pref_size(NTG_OBJECT(box1), border_size1);
    ntg_object_set_border_style(NTG_OBJECT(box1), __create_border1());

    _ntg_container_set_bg(NTG_CONTAINER(box1), nt_color_new(0, 0, 0));

    ntg_box_add_child(box1, NTG_OBJECT(pb1));
    ntg_box_add_child(box1, NTG_OBJECT(cb2));
    ntg_box_add_child(box1, NTG_OBJECT(cb3));
    ntg_box_add_child(box1, NTG_OBJECT(cb4));

    ntg_box* box2 = ntg_box_new(NTG_ORIENTATION_VERTICAL,
            NTG_BOX_ALIGNMENT_1,
            NTG_BOX_ALIGNMENT_1);

    ntg_box_set_bg_color(box2, nt_color_new(255, 255, 255));

    ntg_box_add_child(box2, NTG_OBJECT(cb5));
    ntg_box_add_child(box2, NTG_OBJECT(cb6));

    ntg_border_container* bc = ntg_border_container_new();

    ntg_border_container_set_north(bc, NTG_OBJECT(box1));
    ntg_border_container_set_east(bc, NTG_OBJECT(box2));
    ntg_border_container_set_south(bc, NTG_OBJECT(cb7));
    ntg_border_container_set_west(bc, NTG_OBJECT(cb8));
    ntg_border_container_set_center(bc, NTG_OBJECT(cb9));

    ntg_simple_scene* s1 = ntg_simple_scene_new();
    ntg_scene* _s1 = NTG_SCENE(s1);

    struct ntg_event_sub sub1 = {
        .subscriber = NTG_EVENT_PARTICIPANT_APP,
        .handler = __stage_render_handler
    };

    struct ntg_event_sub sub2 = {
        .subscriber = NTG_EVENT_PARTICIPANT_APP,
        .handler = __scene_layout_handler
    };

    ntg_stage_listen(_main_stage, sub1);
    ntg_scene_listen(_s1, sub2);

    ntg_scene_set_root(NTG_SCENE(s1), NTG_OBJECT(NTG_OBJECT(box1)));
    ntg_scene_set_root(NTG_SCENE(s1), NTG_OBJECT(bc));
    ntg_stage_set_scene(_main_stage, NTG_SCENE(s1));
    ntg_scene_focus(NTG_SCENE(s1), NTG_OBJECT(pb1));

    ntg_loop(_main_stage, 60);

    ntg_prog_bar_destroy(pb1);
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
}

void gui_fn2(ntg_stage* _main_stage, void* data)
{
    ntg_color_block* cb1 = ntg_color_block_new(nt_color_new(255, 0, 0));
    ntg_object* _cb1 = NTG_OBJECT(cb1);

    ntg_object_set_pref_size(_cb1, ntg_xy(10, 10));

    ntg_border_container* bc = ntg_border_container_new();
    ntg_object* _bc = NTG_OBJECT(bc);

    ntg_border_container_set_north(bc, _cb1);

    _ntg_container_set_bg((ntg_container*)bc, nt_color_new(255, 255, 255));

    ntg_simple_scene* s1 = ntg_simple_scene_new();
    ntg_scene* _s1 = NTG_SCENE(s1);

    ntg_scene_set_root(_s1, _bc);
    ntg_stage_set_scene(_main_stage, _s1);

    ntg_loop(_main_stage, 500);

    ntg_color_block_destroy(cb1);
    ntg_simple_scene_destroy(s1);

    return;
}

int main(int argc, char *argv[])
{
    __ntg_init__();

    ntg_simple_stage* main_stage = ntg_simple_stage_new();

    ntg_launch(NTG_STAGE(main_stage), gui_fn1, NULL);

    ntg_wait();

    ntg_simple_stage_destroy(main_stage);

    ntg_destroy();

    return 0;
}
