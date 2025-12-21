#include <assert.h>

#include "core/scene/_ntg_scene_graph.h"
#include "core/object/shared/ntg_object_drawing.h"
// #include "core/scene/shared/ntg_const_object_vec.h"
#include "core/object/shared/ntg_object_vec.h"
#include "shared/_uthash.h"

struct ntg_scene_data_hh
{
    const ntg_object* key;
    struct ntg_scene_node_protect data;
    UT_hash_handle hh;
};

static void _ntg_scene_data_hh_init_(struct ntg_scene_data_hh* data_hh,
        const ntg_object* key)
{
    data_hh->key = key;
    data_hh->data = (struct ntg_scene_node_protect) {0};
    data_hh->data.drawing = ntg_object_drawing_new();
}

static void _ntg_scene_data_hh_deinit_(struct ntg_scene_data_hh* data_hh)
{
    data_hh->key = NULL;
    ntg_object_drawing_destroy(data_hh->data.drawing);
    data_hh->data = (struct ntg_scene_node_protect) {0};
}

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
        _ntg_scene_data_hh_deinit_(current);
        free(current);
    }

    free(scene_graph);
}

void ntg_scene_graph_add(ntg_scene_graph* scene_graph, const ntg_object* object)
{
    assert(scene_graph != NULL);

    struct ntg_scene_data_hh* found = graph_get(scene_graph, object);
    assert(found == NULL);

    struct ntg_scene_data_hh* new = (struct ntg_scene_data_hh*)malloc(
            sizeof(struct ntg_scene_data_hh));
    assert(new != NULL);

    _ntg_scene_data_hh_init_(new, object);

    HASH_ADD_PTR(scene_graph->head, key, new);
}

void ntg_scene_graph_remove(ntg_scene_graph* scene_graph, const ntg_object* object)
{
    assert(scene_graph != NULL);

    struct ntg_scene_data_hh* found = graph_get(scene_graph, object);
    assert(found != NULL);

    HASH_DEL(scene_graph->head, found);
    _ntg_scene_data_hh_deinit_(found);
    free(found);
}

struct ntg_scene_node_protect* ntg_scene_graph_get(
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
