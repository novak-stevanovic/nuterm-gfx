#include <stdlib.h>
#include <assert.h>

#include "core/entity/shared/ntg_event_obs_vec.h"
#include "core/entity/shared/ntg_entity_vec.h"
#include "ntg.h"
#include "shared/_uthash.h"

struct entity_data
{
    unsigned int id;
    ntg_event_obs_vec observers;
};

struct entity_data_hh
{
    ntg_entity* key;
    struct entity_data data;
    UT_hash_handle hh;
};

static void system_map_add(ntg_entity_system* system, ntg_entity* entity, unsigned int id);
static struct entity_data* system_map_get(ntg_entity_system* system, const ntg_entity* entity);
static void system_map_del(ntg_entity_system* system, ntg_entity* entity);
static void system_map_del_all(ntg_entity_system* system);
static void system_map_get_keys(ntg_entity_system* system, ntg_entity_vec* out_vec);

struct ntg_entity_system
{
    unsigned int entity_id_gen;
    unsigned int event_id_gen;
    struct entity_data_hh* map;
};

ntg_entity_system* _ntg_entity_system_new()
{
    ntg_entity_system* new = (ntg_entity_system*)malloc(sizeof(ntg_entity_system));
    assert(new != NULL);

    new->entity_id_gen = 0;
    new->event_id_gen = 0;
    new->map = NULL;

    return new;
}

void _ntg_entity_system_destroy(ntg_entity_system* system)
{
    assert(system != NULL);

    /* destroy entities */
    ntg_entity_vec _vec;
    ntg_entity_vec_init(&_vec);
    system_map_get_keys(system, &_vec);
    size_t i;
    for(i = 0; i < _vec._count; i++)
        ntg_entity_destroy(_vec._data[i]);
    ntg_entity_vec_deinit(&_vec);

    system_map_del_all(system);

    system->map = NULL;

    free(system);
}

void _ntg_entity_system_register(ntg_entity_system* system, ntg_entity* entity)
{
    assert(system != NULL);
    assert(entity != NULL);

    struct entity_data* map_data = system_map_get(system, entity);
    assert(map_data == NULL);

    system_map_add(system, entity, (++system->entity_id_gen));
}

void _ntg_entity_system_unregister(ntg_entity_system* system, ntg_entity* entity)
{
    assert(system != NULL);
    assert(entity != NULL);

    struct entity_data* map_data = system_map_get(system, entity);
    assert(map_data != NULL);

    system_map_del(system, entity);
}

unsigned int _ntg_entity_system_get_id(ntg_entity_system* system, const ntg_entity* entity)
{
    assert(system != NULL);
    assert(entity != NULL);

    struct entity_data* map_data = system_map_get(system, entity);
    assert(map_data != NULL);

    return map_data->id;
}

void _ntg_entity_system_raise_event(
        ntg_entity_system* system,
        ntg_entity* source, 
        ntg_entity* target,
        unsigned int type, void* data)
{
    assert(system != NULL);
    assert(source != NULL);

    struct ntg_event event = {
        .id = (++system->event_id_gen),
        .type = type,
        .source = source,
        .data = data
    };

    struct entity_data* map_data = system_map_get(system, source);
    assert(map_data != NULL);

    ntg_event_obs_vec* subs = &map_data->observers;

    size_t i;
    struct ntg_event_obs it_obs;
    for(i = 0; i < subs->_count; i++)
    {
        it_obs = subs->_data[i];
        struct entity_data* it_map_data = system_map_get(system, it_obs.observer);

        // not deinited & (broadcast or target)
        if((it_map_data != NULL) && ((target == NULL) || (target == it_obs.observer)))
            it_obs.handler(it_obs.observer, event);
    }
}

void _ntg_entity_system_add_observe(
        ntg_entity_system* system,
        ntg_entity* observer,
        ntg_entity* observed,
        ntg_event_handler_fn handler_fn)
{
    assert(system != NULL);
    assert(observer != NULL);
    assert(observed != NULL);
    assert(handler_fn != NULL);

    struct entity_data* observed_data = system_map_get(system, observed);
    struct entity_data* observer_data = system_map_get(system, observer);
    assert(observer_data != NULL);
    assert(observed_data != NULL);

    struct ntg_event_obs obs = {
        .observer = observer,
        .handler = handler_fn
    };

    ntg_event_obs_vec* subs = &observed_data->observers;
    if(observer == observed)
    {
        size_t i;
        struct ntg_event_obs it_sub;
        for(i = 0; i < subs->_count; i++)
        {
            it_sub = subs->_data[i];
            if(it_sub.observer != observer) break;
        }
        ntg_event_obs_vec_ins(subs, obs, i);
    }
    else
    {
        ntg_event_obs_vec_add(subs, obs);
    }
}

void _ntg_entity_system_rm_observe(
        ntg_entity_system* system,
        ntg_entity* observer,
        ntg_entity* observed,
        ntg_event_handler_fn handler_fn)
{
    assert(system != NULL);
    assert(observer != NULL);
    assert(observed != NULL);
    assert(handler_fn != NULL);

    struct entity_data* observed_data = system_map_get(system, observed);
    struct entity_data* observer_data = system_map_get(system, observer);
    assert(observer_data != NULL);

    struct ntg_event_obs obs = {
        .observer = observer,
        .handler = handler_fn
    };

    if(observed_data != NULL)
    {
        if(ntg_event_obs_vec_has(&observed_data->observers, obs))
            ntg_event_obs_vec_rm(&observed_data->observers, obs);
    }
}

bool _ntg_entity_system_has_observe(
        ntg_entity_system* system,
        ntg_entity* observer,
        ntg_entity* observed,
        ntg_event_handler_fn handler_fn)
{
    assert(system != NULL);
    assert(observer != NULL);
    assert(observed != NULL);
    assert(handler_fn != NULL);

    struct entity_data* observed_data = system_map_get(system, observed);
    struct entity_data* observer_data = system_map_get(system, observer);
    assert(observer_data != NULL);
    assert(observed_data != NULL);

    struct ntg_event_obs obs = {
        .observer = observer,
        .handler = handler_fn
    };

    if(observed_data != NULL)
        return ntg_event_obs_vec_has(&observer_data->observers, obs);
    else
        return false;
}

static void system_map_add(ntg_entity_system* system, ntg_entity* entity, unsigned int id)
{
    struct entity_data_hh* data_hh = malloc(sizeof(struct entity_data_hh));
    data_hh->key = entity;
    data_hh->data.id = id;
    ntg_event_obs_vec_init(&data_hh->data.observers);
    HASH_ADD_PTR(system->map, key, data_hh);
}

static struct entity_data* system_map_get(ntg_entity_system* system, const ntg_entity* entity)
{
    struct entity_data_hh* found;
    HASH_FIND_PTR(system->map, &entity, found);

    return (found != NULL) ? &found->data : NULL;
}

static void system_map_del(ntg_entity_system* system, ntg_entity* entity)
{
    struct entity_data_hh* found;
    HASH_FIND_PTR(system->map, &entity, found);

    if(found == NULL) return;
    HASH_DEL(system->map, found);
    ntg_event_obs_vec_deinit(&found->data.observers);
    free(found);
}

static void system_map_del_all(ntg_entity_system* system)
{
    struct entity_data_hh *curr, *tmp;

    HASH_ITER(hh, system->map, curr, tmp)
    {
        HASH_DEL(system->map, curr);
        ntg_event_obs_vec_deinit(&curr->data.observers);
        free(curr);
    };
}

static void system_map_get_keys(ntg_entity_system* system, ntg_entity_vec* out_vec)
{
    struct entity_data_hh *curr, *tmp;

    HASH_ITER(hh, system->map, curr, tmp)
    {
        ntg_entity_vec_add(out_vec, curr->key);
    };
}
