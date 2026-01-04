#include "ntg.h"

#include "base/entity/shared/_ntg_entity_map.h"
#include "base/entity/shared/_ntg_event_sub_vec.h"
#include "base/entity/shared/ntg_entity_vec.h"
#include "shared/_uthash.h"
#include <assert.h>

struct ntg_entity_data_hh
{
    ntg_entity* key;
    struct ntg_entity_data data;
    UT_hash_handle hh;
};

struct ntg_entity_map
{
    struct ntg_entity_data_hh* head;
};

static struct ntg_entity_data_hh* map_get(ntg_entity_map* map, const ntg_entity* entity);

/* -------------------------------------------------------------------------- */

ntg_entity_map* ntg_entity_map_new()
{
    ntg_entity_map* new = (ntg_entity_map*)malloc(sizeof(ntg_entity_map));
    assert(new != NULL);

    new->head = NULL;

    return new;
}

void ntg_entity_map_destroy(ntg_entity_map* map)
{
    assert(map != NULL);

    struct ntg_entity_data_hh *current, *tmp;

    HASH_ITER(hh, map->head, current, tmp) {
        HASH_DEL(map->head, current);  /* delete; users advances to next */

        current->key = NULL;
        ntg_event_sub_vec_destroy(current->data.subscribers);
        current->data = (struct ntg_entity_data) {0};

        free(current);
    }

    free(map);
}

void ntg_entity_map_add(ntg_entity_map* map, ntg_entity* object, unsigned int id)
{
    assert(map != NULL);
    assert(object != NULL);

    struct ntg_entity_data_hh* found = map_get(map, object);
    assert(found == NULL);

    struct ntg_entity_data_hh* new = (struct ntg_entity_data_hh*)malloc(
            sizeof(struct ntg_entity_data_hh));
    assert(new != NULL);

    new->key = object;
    new->data = (struct ntg_entity_data) {0};
    new->data.subscribers = ntg_event_sub_vec_new();
    new->data.id = id;

    HASH_ADD_PTR(map->head, key, new);
}

void ntg_entity_map_remove(ntg_entity_map* map, ntg_entity* entity)
{
    assert(map != NULL);

    struct ntg_entity_data_hh* found = map_get(map, entity);
    assert(found != NULL);

    HASH_DEL(map->head, found);

    found->key = NULL;
    ntg_event_sub_vec_destroy(found->data.subscribers);
    found->data = (struct ntg_entity_data) {0};

    free(found);
}

struct ntg_entity_data* ntg_entity_map_get(
        ntg_entity_map* map,
        const ntg_entity* entity)
{
    struct ntg_entity_data_hh* found = map_get(map, entity);

    return ((found != NULL) ? &(found->data) : NULL);
}

void ntg_entity_map_get_keys(const ntg_entity_map* map, ntg_entity_vec* out_vec)
{
    assert(map != NULL);
    assert(out_vec != NULL);

    struct ntg_entity_data_hh *current, *tmp;

    HASH_ITER(hh, map->head, current, tmp) {
        ntg_entity_vec_append(out_vec, current->key);
    }
}

/* -------------------------------------------------------------------------- */

static struct ntg_entity_data_hh* map_get(
        ntg_entity_map* map,
        const ntg_entity* entity)
{
    assert(map != NULL);

    struct ntg_entity_data_hh* found = NULL;

    HASH_FIND_PTR(map->head, &entity, found);

    return found;
}
