#include "core/ntg_simple_stage.h"
#include "core/ntg_simple_scene.h"
#include "core/ntg_stage.h"
#include "ntg.h"
#include "core/ntg_scene.h"
#include "object/ntg_border_container.h"
#include "object/ntg_color_block.h"
#include <stdio.h>
#include <unistd.h>
#include "object/ntg_container.h"
#include "object/ntg_object.h"
#include "object/ntg_box.h"
#include "shared/ntg_log.h"
#include "shared/ntg_vector.h"

// // TODO: remove
// #include "object/def/ntg_object_def.h"

ntg_color_block* cb1;
ntg_object* _cb1;

void init_gui_func(void* data)
{
    ntg_stage* _main_stage = ntg_get_stage();

    ntg_color_block* cb1 = ntg_color_block_new(nt_color_new(255, 0, 0));
    ntg_object* _cb1 = NTG_OBJECT(cb1);

    ntg_object_set_pref_size(_cb1, ntg_xy(0, 20));

    ntg_color_block* cb2 = ntg_color_block_new(nt_color_new(0, 255, 0));
    ntg_object* _cb2 = NTG_OBJECT(cb2);

    ntg_object_set_pref_size(_cb2, ntg_xy(100, 0));

    ntg_color_block* cb3 = ntg_color_block_new(nt_color_new(0, 0, 255));
    ntg_object* _cb3 = NTG_OBJECT(cb3);

    ntg_object_set_pref_size(_cb3, ntg_xy(0, 5));

    ntg_color_block* cb4 = ntg_color_block_new(nt_color_new(122, 122, 122));
    ntg_object* _cb4 = NTG_OBJECT(cb4);

    ntg_object_set_pref_size(_cb4, ntg_xy(20, 0));

    ntg_color_block* cb5 = ntg_color_block_new(nt_color_new(222, 222, 222));
    ntg_object* _cb5 = NTG_OBJECT(cb5);

    ntg_object_set_pref_size(_cb5, ntg_xy(34, 3));

    ntg_border_container* bc = ntg_border_container_new();
    ntg_object* _bc = NTG_OBJECT(bc);

    ntg_border_container_set_north(bc, _cb1);
    ntg_border_container_set_east(bc, _cb2);
    ntg_border_container_set_south(bc, _cb3);
    ntg_border_container_set_west(bc, _cb4);
    ntg_border_container_set_center(bc, _cb5);

    ntg_simple_scene* s1 = ntg_simple_scene_new(ntg_scene_process_key_fn_def);
    ntg_scene* _s1 = NTG_SCENE(s1);
    ntg_scene_set_root(_s1, _bc);
    ntg_stage_set_scene(_main_stage, _s1);

    ntg_loop(NTG_FRAMERATE_DEFAULT);

    ntg_color_block_destroy(cb1);
    ntg_color_block_destroy(cb2);
    ntg_color_block_destroy(cb3);
    ntg_color_block_destroy(cb4);
    ntg_color_block_destroy(cb5);
    ntg_border_container_destroy(bc);

    ntg_simple_scene_destroy(s1);

    return;
}

int main(int argc, char *argv[])
{
    ntg_simple_stage* main_stage = ntg_simple_stage_new(
            ntg_stage_process_key_fn_def);

    ntg_initialize(NTG_STAGE(main_stage), init_gui_func, NULL);

    ntg_launch();
    // printf("\rMT: Launched NTG thread - STATUS: %d\n", _status);

    // printf("\rMT: Waiting for NTG thread.\n");
    ntg_destroy();

    // TODO: shouldnt be after ntg_destroy()
    ntg_simple_stage_destroy(main_stage);

    // printf("\rMT: Done. Exiting.\n");

    // int n1[] = { 1, 2 };
    // int* n2 = { 1, 2 };
    //
    return 0;
}
