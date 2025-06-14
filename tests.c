#include "core/ntg_stage.h"
#include "ntg.h"
#include "core/ntg_scene.h"
#include "object/ntg_color_block.h"
#include <unistd.h>
#include "object/ntg_container.h"
#include "object/ntg_object.h"
#include "object/ntg_box.h"
#include "shared/ntg_vector.h"

/*
event thread:
{
    event e = wait_for_user_input();
    add_to_event_queue(e);
}

main thread:
{
    event e = wait_for_event_queue();
    if(e.key_pressed = 'q') quit;
    else if ...
}

 */

void init_gui_func(void* data)
{
    ntg_color_block_t* cb1 = ntg_color_block_new(nt_color_new(255, 0, 90));
    ntg_object_t* _cb1 = (ntg_object_t*)cb1;
    ntg_object_set_pref_size(_cb1, ntg_xy(50, 20));

    ntg_color_block_t* cb2 = ntg_color_block_new(nt_color_new(0, 200, 0));
    ntg_object_t* _cb2 = (ntg_object_t*)cb2;
    ntg_object_set_pref_size(_cb2, ntg_xy(100, 10));

    ntg_color_block_t* cb3 = ntg_color_block_new(nt_color_new(0, 100, 0));
    ntg_object_t* _cb3 = (ntg_object_t*)cb3;
    ntg_object_set_pref_size(_cb3, ntg_xy(60, 5));

    ntg_color_block_t* cb4 = ntg_color_block_new(nt_color_new(0, 100, 200));
    ntg_object_t* _cb4 = (ntg_object_t*)cb4;
    ntg_object_set_pref_size(_cb4, ntg_xy(30, 10));

    ntg_box_t* box1 = ntg_box_new(NTG_BOX_ORIENTATION_HORIZONTAL);
    ntg_object_t* _box1 = (ntg_object_t*)box1;

    ntg_box_t* box2 = ntg_box_new(NTG_BOX_ORIENTATION_VERTICAL);
    ntg_object_t* _box2 = (ntg_object_t*)box2;

    ntg_box_add_child(box1, _cb1);
    ntg_box_add_child(box1, _box2);

    ntg_box_add_child(box2, _cb2);
    ntg_box_add_child(box2, _cb3);
    ntg_box_add_child(box2, _cb4);

    _ntg_container_set_bg((ntg_container_t*)box2, nt_color_new(255, 255, 255));

    ntg_scene_t* s = ntg_scene_new();
    ntg_scene_set_root(s, _box1);
    ntg_stage_set_scene(s);

    ntg_loop();

    ntg_color_block_destroy(cb1);
    ntg_color_block_destroy(cb2);
    ntg_color_block_destroy(cb3);
    ntg_box_destroy(box1);
    // ntg_box_destroy(box2);
    ntg_scene_destroy(s);
    return;
}

int main(int argc, char *argv[])
{
    ntg_status_t _status;

    ntg_launch(init_gui_func, NULL, &_status);
    // printf("\rMT: Launched NTG thread - STATUS: %d\n", _status);

    // printf("\rMT: Waiting for NTG thread.\n");
    ntg_destroy();
    // printf("\rMT: Done. Exiting.\n");

    // int n1[] = { 1, 2 };
    // int* n2 = { 1, 2 };
    //
    return 0;
}
