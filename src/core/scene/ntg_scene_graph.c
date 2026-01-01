#include <assert.h>

#include "core/scene/ntg_scene_graph.h"
#include "core/object/shared/ntg_object_drawing.h"
// #include "core/scene/shared/ntg_const_object_vec.h"
#include "core/object/shared/ntg_object_vec.h"
#include "shared/_uthash.h"

struct ntg_scene_data_hh
{
    const ntg_object* key;
    struct ntg_scene_node_pr data;
    UT_hash_handle hh;
};

/* -------------------------------------------------------------------------- */

struct ntg_scene_graph
{
    struct ntg_scene_data_hh* head;
};

static struct ntg_scene_data_hh* graph_get(
        ntg_scene_graph* scene_graph,
        const ntg_object* object);

ntg_scene_graph* ntg_scene_graph_new()
{
    ntg_scene_graph* new = (ntg_scene_graph*)malloc(sizeof(ntg_scene_graph));
    assert(new != NULL);

    new->head = NULL;

    return new;
}

void ntg_scene_graph_destroy(ntg_scene_graph* scene_graph)
{
    assert(scene_graph != NULL);

    struct ntg_scene_data_hh *current, *tmp;

    HASH_ITER(hh, scene_graph->head, current, tmp) {
        HASH_DEL(scene_graph->head, current);  /* delete; users advances to next */

        current->key = NULL;
        ntg_object_drawing_destroy(current->data.drawing);
        free(current->data.object_layout_data);
        free(current->data.data);
        current->data = (struct ntg_scene_node_pr) {0};

        free(current);
    }

    free(scene_graph);
}

struct ntg_scene_node_pr* ntg_scene_graph_add(
        ntg_scene_graph* scene_graph,
        const ntg_object* object)
{
    assert(scene_graph != NULL);

    struct ntg_scene_data_hh* found = graph_get(scene_graph, object);
    assert(found == NULL);

    struct ntg_scene_data_hh* new = (struct ntg_scene_data_hh*)malloc(
            sizeof(struct ntg_scene_data_hh));
    assert(new != NULL);

    new->key = object;
    new->data = (struct ntg_scene_node_pr) {0};
    new->data.drawing = ntg_object_drawing_new();

    HASH_ADD_PTR(scene_graph->head, key, new);

    return &(new->data);
}

void ntg_scene_graph_remove(
        ntg_scene_graph* scene_graph,
        const ntg_object* object)
{
    assert(scene_graph != NULL);

    struct ntg_scene_data_hh* found = graph_get(scene_graph, object);
    assert(found != NULL);

    HASH_DEL(scene_graph->head, found);

    found->key = NULL;
    ntg_object_drawing_destroy(found->data.drawing);
    found->data = (struct ntg_scene_node_pr) {0};

    free(found);
}

struct ntg_scene_node_pr* ntg_scene_graph_get(
        ntg_scene_graph* scene_graph,
        const ntg_object* object)
{
    struct ntg_scene_data_hh* found = graph_get(scene_graph, object);

    return ((found != NULL) ? &(found->data) : NULL);
}

void ntg_scene_graph_get_keys(
        const ntg_scene_graph* scene_graph,
        ntg_const_object_vec* out_vec)
{
    assert(scene_graph != NULL);
    assert(out_vec != NULL);

    struct ntg_scene_data_hh *current, *tmp;

    HASH_ITER(hh, scene_graph->head, current, tmp) {
        ntg_const_object_vec_append(out_vec, current->key);
    }
}

/* -------------------------------------------------------------------------- */

static struct ntg_scene_data_hh* graph_get(
        ntg_scene_graph* scene_graph,
        const ntg_object* object)
{
    assert(scene_graph != NULL);

    struct ntg_scene_data_hh* found = NULL;

    HASH_FIND_PTR(scene_graph->head, &object, found);

    return found;
}
