#include <assert.h>

#include "core/scene/_ntg_scene_graph.h"
#include "core/scene/shared/_ntg_drawing.h"
#include "core/scene/shared/ntg_cdrawable_vec.h"
#include "shared/_uthash.h"

struct ntg_scene_data_hh
{
    const ntg_drawable* key;
    struct _ntg_scene_node data;
    UT_hash_handle hh;
};

static void __ntg_scene_data_hh_init__(struct ntg_scene_data_hh* data_hh,
        const ntg_drawable* key)
{
    data_hh->key = key;
    data_hh->data = (struct _ntg_scene_node) {0};
    data_hh->data.drawing = ntg_drawing_new();
}

static void __ntg_scene_data_hh_deinit__(struct ntg_scene_data_hh* data_hh)
{
    data_hh->key = NULL;
    ntg_drawing_destroy(data_hh->data.drawing);
    data_hh->data = (struct _ntg_scene_node) {0};
}

struct ntg_scene_graph
{
    struct ntg_scene_data_hh* head;
};

static struct ntg_scene_data_hh* __ntg_scene_graph_get(
        ntg_scene_graph* scene_graph,
        const ntg_drawable* drawable);

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
        __ntg_scene_data_hh_deinit__(current);
        free(current);
    }

    free(scene_graph);
}

void ntg_scene_graph_add(ntg_scene_graph* scene_graph, const ntg_drawable* drawable)
{
    assert(scene_graph != NULL);

    struct ntg_scene_data_hh* found = __ntg_scene_graph_get(scene_graph, drawable);
    assert(found == NULL);

    struct ntg_scene_data_hh* new = (struct ntg_scene_data_hh*)malloc(
            sizeof(struct ntg_scene_data_hh));
    assert(new != NULL);

    __ntg_scene_data_hh_init__(new, drawable);

    HASH_ADD_PTR(scene_graph->head, key, new);
}

void ntg_scene_graph_remove(ntg_scene_graph* scene_graph, const ntg_drawable* drawable)
{
    assert(scene_graph != NULL);

    struct ntg_scene_data_hh* found = __ntg_scene_graph_get(scene_graph, drawable);
    assert(found != NULL);

    HASH_DEL(scene_graph->head, found);
    __ntg_scene_data_hh_deinit__(found);
    free(found);
}

static struct ntg_scene_data_hh* __ntg_scene_graph_get(
        ntg_scene_graph* scene_graph,
        const ntg_drawable* drawable)
{
    assert(scene_graph != NULL);

    struct ntg_scene_data_hh* found = NULL;

    HASH_FIND_PTR(scene_graph->head, &drawable, found);

    return found;
}

struct _ntg_scene_node* ntg_scene_graph_get(
        ntg_scene_graph* scene_graph,
        const ntg_drawable* drawable)
{
    struct ntg_scene_data_hh* found = __ntg_scene_graph_get(scene_graph, drawable);

    return ((found != NULL) ? &(found->data) : NULL);
}

void ntg_scene_graph_get_keys(
        const ntg_scene_graph* scene_graph,
        ntg_cdrawable_vec* out_vec)
{
    assert(scene_graph != NULL);
    assert(out_vec != NULL);

    struct ntg_scene_data_hh *current, *tmp;

    HASH_ITER(hh, scene_graph->head, current, tmp) {
        ntg_cdrawable_vec_append(out_vec, current->key);
    }
}
