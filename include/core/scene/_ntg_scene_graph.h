#ifndef _NTG_SCENE_GRAPH_H_
#define _NTG_SCENE_GRAPH_H_

#include "shared/ntg_xy.h"

typedef struct ntg_scene_graph ntg_scene_graph;
typedef struct ntg_drawable ntg_drawable;
typedef struct ntg_drawing ntg_drawing;

// TODO:
struct ntg_scene_data
{
    struct ntg_xy min_size, natural_size, max_size;

    struct ntg_xy size;

    struct ntg_xy position;

    ntg_drawing* drawing;
};

ntg_scene_graph* ntg_scene_graph_new();
void ntg_scene_graph_destroy(ntg_scene_graph* scene_graph);

void ntg_scene_graph_add(ntg_scene_graph* scene_graph, ntg_drawable* drawable);
void ntg_scene_graph_remove(ntg_scene_graph* scene_graph, ntg_drawable* drawable);
struct ntg_scene_data* ntg_scene_graph_get(ntg_scene_graph* scene_graph,
        ntg_drawable* drawable);

#endif // _NTG_SCENE_GRAPH_H_
