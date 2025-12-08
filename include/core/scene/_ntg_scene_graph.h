#ifndef _NTG_SCENE_GRAPH_H_
#define _NTG_SCENE_GRAPH_H_

#include "shared/ntg_xy.h"

typedef struct ntg_scene_graph ntg_scene_graph;
typedef struct ntg_drawable ntg_drawable;
typedef struct ntg_drawing ntg_drawing;
typedef struct ntg_cdrawable_vec ntg_cdrawable_vec;

struct _ntg_scene_node
{
    struct ntg_xy min_size, natural_size,
                  max_size, grow;

    struct ntg_xy size;
    struct ntg_xy position;
    ntg_drawing* drawing;
    void* data;
};

ntg_scene_graph* ntg_scene_graph_new();
void ntg_scene_graph_destroy(ntg_scene_graph* scene_graph);

void ntg_scene_graph_add(ntg_scene_graph* scene_graph, const ntg_drawable* drawable);
void ntg_scene_graph_remove(ntg_scene_graph* scene_graph, const ntg_drawable* drawable);
struct _ntg_scene_node* ntg_scene_graph_get(ntg_scene_graph* scene_graph,
        const ntg_drawable* drawable);

/* Expects an initialized, empty vector. */
void ntg_scene_graph_get_keys(
        const ntg_scene_graph* scene_graph,
        ntg_cdrawable_vec* out_vec);

#endif // _NTG_SCENE_GRAPH_H_
