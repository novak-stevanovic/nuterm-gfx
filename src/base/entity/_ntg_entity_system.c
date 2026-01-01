#include <assert.h>
#include <stdlib.h>

#include "base/entity/_ntg_entity_system.h"
#include "base/entity/shared/_ntg_entity_map.h"
#include "base/entity/shared/_ntg_event_sub_vec.h"
#include "base/entity/shared/ntg_entity_vec.h"

struct ntg_entity_system
{
    unsigned int entity_id_gen;
    unsigned int event_id_gen;
    ntg_entity_map* map;
};

ntg_entity_system* ntg_entity_system_new()
{
    ntg_entity_system* new = (ntg_entity_system*)malloc(sizeof(ntg_entity_system));
    assert(new != NULL);

    new->entity_id_gen = 0;
    new->event_id_gen = 0;
    new->map = ntg_entity_map_new();

    return new;
}

void ntg_entity_system_destroy(ntg_entity_system* system)
{
    assert(system != NULL);

    /* destroy entities */
    ntg_entity_vec _vec;
    _ntg_entity_vec_init_(&_vec);
    ntg_entity_map_get_keys(system->map, &_vec);
    size_t i;
    for(i = 0; i < _vec._count; i++)
        ntg_entity_destroy(_vec._data[i]);
    _ntg_entity_vec_deinit_(&_vec);

    ntg_entity_map_destroy(system->map);

    free(system);
}

void ntg_entity_system_register(ntg_entity_system* system, ntg_entity* entity)
{
    assert(system != NULL);
    assert(entity != NULL);

    struct ntg_entity_data* map_data = ntg_entity_map_get(system->map, entity);
    assert(map_data == NULL);

    ntg_entity_map_add(system->map, entity, (++system->entity_id_gen));
}

void ntg_entity_system_unregister(ntg_entity_system* system, ntg_entity* entity)
{
    assert(system != NULL);
    assert(entity != NULL);

    struct ntg_entity_data* map_data = ntg_entity_map_get(system->map, entity);
    assert(map_data != NULL);

    ntg_entity_map_remove(system->map, entity);
}

unsigned int ntg_entity_system_get_id(ntg_entity_system* system, const ntg_entity* entity)
{
    assert(system != NULL);
    assert(entity != NULL);

    struct ntg_entity_data* map_data = ntg_entity_map_get(system->map, entity);
    assert(map_data != NULL);

    return map_data->id;
}

void ntg_entity_system_raise_event(
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

    ntg_event_sub_vec* subs = map_data->subscribers;

    size_t i;
    struct ntg_event_sub it_sub;
    for(i = 0; i < subs->_count; i++)
    {
        it_sub = subs->_data[i];
        struct ntg_entity_data* it_map_data = ntg_entity_map_get(system->map, it_sub.entity);

        // not deinited & (broadcast or target)
        if((it_map_data != NULL) && ((target == NULL) || (target == it_sub.entity)))
            it_sub.handler(it_sub.entity, event);
    }
}

void ntg_entity_system_add_observe(
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

    ntg_event_sub_vec* subs = observed_data->subscribers;
    if(observer == observed)
    {
        size_t i;
        struct ntg_event_sub it_sub;
        for(i = 0; i < subs->_count; i++)
        {
            it_sub = subs->_data[i];
            if(it_sub.entity != observer) break;
        }

        ntg_event_sub_vec_insert(subs, observer, handler_fn, i);
    }
    else
    {
        ntg_event_sub_vec_append(subs, observer, handler_fn);
    }
}

void ntg_entity_system_rm_observe(
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

    if(observed_data != NULL)
    {
        if(ntg_event_sub_vec_contains(observed_data->subscribers, observer, handler_fn))
            ntg_event_sub_vec_remove(observed_data->subscribers, observer, handler_fn);
    }
}

bool ntg_entity_system_has_observe(
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

    if(observed_data != NULL)
        return ntg_event_sub_vec_contains(observer_data->subscribers, observer, handler_fn);
    else
        return false;
}
