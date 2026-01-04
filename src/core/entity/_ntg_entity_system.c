#include "core/entity/shared/ntg_entity_map.h"
#include "core/entity/shared/ntg_event_obs_vec.h"
#include "core/entity/shared/ntg_entity_vec.h"
#include "ntg.h"
#include <assert.h>
#include <stdlib.h>

struct ntg_entity_system
{
    unsigned int entity_id_gen;
    unsigned int event_id_gen;
    ntg_entity_map* map;
};

ntg_entity_system* _ntg_entity_system_new()
{
    ntg_entity_system* new = (ntg_entity_system*)malloc(sizeof(ntg_entity_system));
    assert(new != NULL);

    new->entity_id_gen = 0;
    new->event_id_gen = 0;
    new->map = ntg_entity_map_new();

    return new;
}

void _ntg_entity_system_destroy(ntg_entity_system* system)
{
    assert(system != NULL);

    /* destroy entities */
    ntg_entity_vec _vec;
    ntg_entity_vec_init(&_vec);
    ntg_entity_map_get_keys(system->map, &_vec);
    size_t i;
    for(i = 0; i < _vec._count; i++)
        ntg_entity_destroy(_vec._data[i]);
    ntg_entity_vec_deinit(&_vec);

    ntg_entity_map_destroy(system->map);

    free(system);
}

void _ntg_entity_system_register(ntg_entity_system* system, ntg_entity* entity)
{
    assert(system != NULL);
    assert(entity != NULL);

    struct ntg_entity_data* map_data = ntg_entity_map_get(system->map, entity);
    assert(map_data == NULL);

    ntg_entity_map_add(system->map, entity, (++system->entity_id_gen));
}

void _ntg_entity_system_unregister(ntg_entity_system* system, ntg_entity* entity)
{
    assert(system != NULL);
    assert(entity != NULL);

    struct ntg_entity_data* map_data = ntg_entity_map_get(system->map, entity);
    assert(map_data != NULL);

    ntg_entity_map_rm(system->map, entity);
}

unsigned int _ntg_entity_system_get_id(ntg_entity_system* system, const ntg_entity* entity)
{
    assert(system != NULL);
    assert(entity != NULL);

    struct ntg_entity_data* map_data = ntg_entity_map_get(system->map, entity);
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

    struct ntg_entity_data* map_data = ntg_entity_map_get(system->map, source);
    assert(map_data != NULL);

    ntg_event_obs_vec* subs = &map_data->observers;

    size_t i;
    struct ntg_event_obs it_obs;
    for(i = 0; i < subs->_count; i++)
    {
        it_obs = subs->_data[i];
        struct ntg_entity_data* it_map_data = ntg_entity_map_get(system->map, it_obs.observer);

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

    struct ntg_entity_data* observed_data = ntg_entity_map_get(system->map, observed);
    struct ntg_entity_data* observer_data = ntg_entity_map_get(system->map, observer);
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

    struct ntg_entity_data* observed_data = ntg_entity_map_get(system->map, observed);
    struct ntg_entity_data* observer_data = ntg_entity_map_get(system->map, observer);
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

    struct ntg_entity_data* observed_data = ntg_entity_map_get(system->map, observed);
    struct ntg_entity_data* observer_data = ntg_entity_map_get(system->map, observer);
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
