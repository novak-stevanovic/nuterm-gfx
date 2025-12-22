#include <stdlib.h>
#include <assert.h>

#include "base/entity/ntg_entity.h"
#include "base/entity/_ntg_entity_system.h"
#include "base/entity/shared/ntg_entity_vec.h"

void _ntg_entity_init_(ntg_entity* entity, struct ntg_entity_init_data init_data)
{
    assert(entity != NULL);
    assert(init_data.system != NULL);

    entity->type = init_data.type;
    entity->__deinit_fn = (init_data.deinit_fn != NULL) ?
        init_data.deinit_fn : _ntg_entity_deinit_fn;
    entity->__system = init_data.system;
    entity->_arena = (init_data.group != NULL) ?
        ntg_entity_group_get_arena(init_data.group) : NULL;

    ntg_entity_system_register(init_data.system, entity);
}

void _ntg_entity_deinit_(ntg_entity* entity)
{
    _ntg_entity_deinit_fn(entity);
}

void _ntg_entity_deinit_fn(ntg_entity* entity)
{
    assert(entity != NULL);
    
    ntg_entity_system_mark_deinit(entity->__system, entity);
    (*entity) = (ntg_entity) {0};
}

/* ------------------------------------------------------ */

void ntg_entity_raise_event(
        ntg_entity* entity,
        unsigned int type,
        void* data)
{
    ntg_entity_system_raise_event(entity->__system, entity, type, data);
}

void ntg_entity_observe(
        ntg_entity* observer,
        ntg_entity* observed,
        ntg_event_handler_fn handler_fn)
{
    assert(observer->__system == observed->__system);

    ntg_entity_system_add_observe(observer->__system, observer, observed, handler_fn);
}

void ntg_entity_stop_observing(
        ntg_entity* observer,
        ntg_entity* observed,
        ntg_event_handler_fn handler_fn)
{
    ntg_entity_system_rm_observe(observer->__system, observer, observed, handler_fn);
}

bool ntg_entity_is_observing(
        ntg_entity* observer,
        ntg_entity* observed,
        ntg_event_handler_fn handler_fn)
{
    return ntg_entity_system_has_observe(observer->__system,
            observer, observed, handler_fn);
}

/* ------------------------------------------------------ */
/* ENTITY GROUP */
/* ------------------------------------------------------ */

struct ntg_entity_group
{
    ntg_entity_vec entities;
    sarena* arena;
};

ntg_entity_group* ntg_entity_group_new(sarena* arena)
{
    assert(arena != NULL);

    ntg_entity_group* new = (ntg_entity_group*)malloc(sizeof(ntg_entity_group));

    _ntg_entity_vec_init_(&new->entities);
    new->arena = arena;

    return new;
}

void ntg_entity_group_destroy(ntg_entity_group* group)
{
    assert(group != NULL);

    size_t i;
    for(i = 0; i < group->entities._count; i++)
        _ntg_entity_deinit_(group->entities._data[i]);

    _ntg_entity_vec_deinit_(&group->entities);

    free(group);
}
