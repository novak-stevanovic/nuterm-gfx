#include <assert.h>

#include "core/scene/_ntg_scene_graph.h"
#include "core/scene/shared/ntg_drawing.h"
#include "shared/_uthash.h"

struct ntg_scene_data_hh
{
    ntg_drawable* key;
    struct ntg_scene_data data;
    UT_hash_handle hh;
};

static void __ntg_scene_data_hh_init__(struct ntg_scene_data_hh* data_hh,
        ntg_drawable* key)
{
    data_hh->key = key;
    data_hh->data = (struct ntg_scene_data) {0};
    data_hh->data.drawing = ntg_drawing_new();
}

struct ntg_scene_graph
{
    struct ntg_scene_data_hh* graph;
};

static struct ntg_scene_data_hh* __ntg_scene_graph_get(
        ntg_scene_graph* scene_graph,
        ntg_drawable* drawable);

ntg_scene_graph* ntg_scene_graph_new()
{
    ntg_scene_graph* new = (ntg_scene_graph*)malloc(sizeof(ntg_scene_graph));
    assert(new != NULL);

    new->graph = NULL;

    return new;
}

void ntg_scene_graph_destroy(ntg_scene_graph* scene_graph)
{
    assert(scene_graph != NULL);

    // TODO

    free(scene_graph);
}

void ntg_scene_graph_add(ntg_scene_graph* scene_graph, ntg_drawable* drawable)
{
    assert(scene_graph != NULL);

    struct ntg_scene_data_hh* found = __ntg_scene_graph_get(scene_graph, drawable);
    assert(found == NULL);

    struct ntg_scene_data_hh* new = (struct ntg_scene_data_hh*)malloc(
            sizeof(struct ntg_scene_data_hh));
    assert(new != NULL);

    __ntg_scene_data_hh_init__(new, drawable);

    HASH_ADD(hh, scene_graph->graph, key, sizeof(ntg_drawable*), new);
}

void ntg_scene_graph_remove(ntg_scene_graph* scene_graph, ntg_drawable* drawable)
{
    assert(scene_graph != NULL);

    struct ntg_scene_data_hh* found = __ntg_scene_graph_get(scene_graph, drawable);
    assert(found != NULL);

    HASH_DELETE(hh, scene_graph->graph, found);
    
    free(found);
}

static struct ntg_scene_data_hh* __ntg_scene_graph_get(ntg_scene_graph* scene_graph,
        ntg_drawable* drawable)
{
    assert(scene_graph != NULL);

    struct ntg_scene_data_hh* found = NULL;

    HASH_FIND(hh, scene_graph->graph, drawable, sizeof(void*), found);

    return found;
}

struct ntg_scene_data* ntg_scene_graph_get(ntg_scene_graph* scene_graph,
        ntg_drawable* drawable)
{
    struct ntg_scene_data_hh* found = __ntg_scene_graph_get(scene_graph, drawable);

    return ((found != NULL) ? &(found->data) : NULL);
}
