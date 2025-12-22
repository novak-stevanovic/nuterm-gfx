#include <assert.h>
#include <stdlib.h>

#include "base/entity/_ntg_entity_system.h"
#include "base/entity/shared/_ntg_entity_map.h"
#include "base/entity/shared/_ntg_event_sub_vec.h"

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
    new->map = ntg_entity_map_new();

    return new;
}

void ntg_entity_system_destroy(ntg_entity_system* system)
{
    assert(system != NULL);

    ntg_entity_map_destroy(system->map);

    free(system);
}

void ntg_entity_system_register(ntg_entity_system* system, ntg_entity* entity)
{
    assert(system != NULL);
    assert(entity != NULL);

    struct ntg_entity_data* map_data = ntg_entity_map_get(system->map, entity);
    if(map_data != NULL) return;

    ntg_entity_map_add(system->map, entity, (++system->entity_id_gen));
}

unsigned int ntg_entity_system_get_id(ntg_entity_system* system, const ntg_entity* entity)
{
    assert(system != NULL);
    assert(entity != NULL);

    struct ntg_entity_data* map_data = ntg_entity_map_get(system->map, entity);
    assert(map_data != NULL);

    return map_data->id;
}

void ntg_entity_system_mark_deinit(ntg_entity_system* system, const ntg_entity* entity)
{
    assert(system != NULL);
    assert(entity != NULL);

    struct ntg_entity_data* map_data = ntg_entity_map_get(system->map, entity);
    assert(map_data != NULL);

    map_data->deinit = true;
}

void ntg_entity_system_raise_event(
        ntg_entity_system* system,
        ntg_entity* source, 
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

    ntg_event_sub_vec* subs = map_data->subs;

    size_t i;
    struct ntg_entity_data* it_map_data;
    struct ntg_event_sub it_sub;
    for(i = 0; i < subs->_count; i++)
    {
        it_sub = subs->_data[i];
        it_map_data = ntg_entity_map_get(system->map, it_sub.observer);

        if(!it_map_data->deinit)
            it_sub.handler(it_sub.observer, event);
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

    if(observed_data == NULL)
        ntg_entity_system_register(system, observed);
    if(observer_data == NULL)
        ntg_entity_system_register(system, observer);

    ntg_event_sub_vec_append(observed_data->subs, observer, handler_fn);
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

    if(observed_data == NULL)
        ntg_entity_system_register(system, observed);
    if(observer_data == NULL)
        ntg_entity_system_register(system, observer);

    if(ntg_event_sub_vec_contains(observed_data->subs, observer, handler_fn))
        ntg_event_sub_vec_remove(observed_data->subs, observer, handler_fn);
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

    if(observed_data == NULL)
        ntg_entity_system_register(system, observed);
    if(observer_data == NULL)
        ntg_entity_system_register(system, observer);

    return ntg_event_sub_vec_contains(observed_data->subs, observer, handler_fn);
}
