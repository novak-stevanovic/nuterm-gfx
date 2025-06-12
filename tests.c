#include "core/ntg_stage.h"
#include "ntg.h"
#include "core/ntg_scene.h"
#include "object/ntg_color_block.h"
#include <stdio.h>
#include <unistd.h>
#include "object/ntg_object.h"
#include "object/ntg_box.h"
#include "shared/ntg_vector.h"

void init_gui_func(void* data)
{
    ntg_color_block_t* cb1 = ntg_color_block_new(nt_color_new(255, 0, 90));
    ntg_object_t* _cb1 = (ntg_object_t*)cb1;
    ntg_object_set_pref_size(_cb1, ntg_xy(100, 20));

    ntg_color_block_t* cb2 = ntg_color_block_new(nt_color_new(0, 200, 0));
    ntg_object_t* _cb2 = (ntg_object_t*)cb2;
    ntg_object_set_pref_size(_cb2, ntg_xy(50, 40));

    ntg_box_t* box = ntg_box_new();
    ntg_object_t* _box = (ntg_object_t*)box;
    ntg_box_add_child(box, _cb1);
    ntg_box_add_child(box, _cb2);

    ntg_scene_t* s = ntg_scene_new();
    ntg_scene_set_root(s, _box);
    ntg_stage_set_scene(s);

    ntg_loop();

    ntg_color_block_destroy(cb1);
    ntg_color_block_destroy(cb2);
    ntg_box_destroy(box);
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
