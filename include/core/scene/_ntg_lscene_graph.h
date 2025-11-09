#ifndef _NTG_LSCENE_GRAPH_H_
#define _NTG_LSCENE_GRAPH_H_

#include "shared/ntg_xy.h"

typedef struct ntg_lscene_graph ntg_lscene_graph;
typedef struct ntg_drawable ntg_drawable;
typedef struct ntg_drawing ntg_drawing;

struct ntg_lscene_node
{
    struct ntg_xy min_size, natural_size,
                  max_size, grow;

    struct ntg_xy size;
    struct ntg_xy position;
    ntg_drawing* drawing;
};

ntg_lscene_graph* ntg_lscene_graph_new();
void ntg_lscene_graph_destroy(ntg_lscene_graph* scene_graph);

void ntg_lscene_graph_add(ntg_lscene_graph* scene_graph, const ntg_drawable* drawable);
void ntg_lscene_graph_remove(ntg_lscene_graph* scene_graph, const ntg_drawable* drawable);
struct ntg_lscene_node* ntg_lscene_graph_get(ntg_lscene_graph* scene_graph,
        const ntg_drawable* drawable);

#endif // _NTG_SCENE_GRAPH_H_
