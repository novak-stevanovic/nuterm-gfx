#ifndef _NTG_LSCENE_H_
#define _NTG_LSCENE_H_

#include "core/scene/ntg_scene.h"

typedef struct ntg_lscene ntg_lscene;
typedef struct ntg_lscene_graph ntg_lscene_graph;

struct ntg_lscene
{
    ntg_scene __base;
    
    ntg_lscene_graph* __graph;
};

void __ntg_lscene_init__(
        ntg_lscene* scene,
        ntg_scene_process_key_fn process_key_fn);
void __ntg_lscene_deinit__(ntg_lscene* scene);

#endif // _NTG_LSCENE_H_
