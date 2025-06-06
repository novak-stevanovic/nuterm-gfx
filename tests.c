#include "core/ntg_stage.h"
#include "ntg.h"
#include "core/ntg_scene.h"
#include "object/ntg_color_block.h"
#include <stdio.h>
#include <unistd.h>

void init_gui_func(void* data)
{
    ntg_color_block_t* cb = ntg_color_block_new(nt_color_new(255, 0, 90));

    ntg_scene_t* s = ntg_scene_new();
    ntg_scene_set_root(s, (ntg_object_t*)cb);

    ntg_stage_set_scene(s);

    ntg_stage_render();

    ntg_color_block_destroy(cb);
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
