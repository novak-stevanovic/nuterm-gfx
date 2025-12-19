#ifndef _NTG_SCENE_GRAPH_H_
#define _NTG_SCENE_GRAPH_H_

#include "shared/ntg_xy.h"

typedef struct ntg_scene_graph ntg_scene_graph;
typedef struct ntg_object ntg_object;
typedef struct ntg_object_drawing ntg_object_drawing;
typedef struct ntg_cobject_vec ntg_cobject_vec;

struct ntg_scene_node_protect
{
    struct ntg_xy min_size, natural_size,
                  max_size, grow;

    struct ntg_xy size;
    struct ntg_xy position;
    ntg_object_drawing* drawing;
    bool layout_init;
    void* object_layout_data;
    void* data;
};

ntg_scene_graph* ntg_scene_graph_new();
void ntg_scene_graph_destroy(ntg_scene_graph* scene_graph);

void ntg_scene_graph_add(ntg_scene_graph* scene_graph, const ntg_object* object);
void ntg_scene_graph_remove(ntg_scene_graph* scene_graph, const ntg_object* object);
struct ntg_scene_node_protect* ntg_scene_graph_get(ntg_scene_graph* scene_graph,
        const ntg_object* object);

/* Expects an initialized, empty vector. */
void ntg_scene_graph_get_keys(
        const ntg_scene_graph* scene_graph,
        ntg_cobject_vec* out_vec);

#endif // _NTG_SCENE_GRAPH_H_
