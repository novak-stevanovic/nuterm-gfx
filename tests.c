#include "ntg.h"
#include "ntg_core/ntg_scene.h"
#include <unistd.h>

void init_gui_func(void* data)
{
    ntg_scene_t* scene1 = ntg_scene_new();
    ntg_scene_t* scene2 = ntg_scene_new();

    ntg_scene_destroy(scene1);
    ntg_scene_destroy(scene2);
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
