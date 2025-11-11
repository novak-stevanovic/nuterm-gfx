#ifndef _NTG_SIMPLE_SCENE_GRAPH_H_
#define _NTG_SIMPLE_SCENE_GRAPH_H_

#include "shared/ntg_xy.h"

typedef struct ntg_simple_scene_graph ntg_simple_scene_graph;
typedef struct ntg_drawable ntg_drawable;
typedef struct ntg_drawing ntg_drawing;

struct ntg_simple_scene_node
{
    struct ntg_xy min_size, natural_size,
                  max_size, grow;

    struct ntg_xy size;
    struct ntg_xy position;
    ntg_drawing* drawing;
};

ntg_simple_scene_graph* ntg_simple_scene_graph_new();
void ntg_simple_scene_graph_destroy(ntg_simple_scene_graph* scene_graph);

void ntg_simple_scene_graph_add(ntg_simple_scene_graph* scene_graph, const ntg_drawable* drawable);
void ntg_simple_scene_graph_remove(ntg_simple_scene_graph* scene_graph, const ntg_drawable* drawable);
struct ntg_simple_scene_node* ntg_simple_scene_graph_get(ntg_simple_scene_graph* scene_graph,
        const ntg_drawable* drawable);

#endif // _NTG_SIMPLE_SCENE_GRAPH_H_
