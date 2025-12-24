#include <stdlib.h>
#include <assert.h>

#include "base/entity/ntg_entity.h"
#include "base/entity/_ntg_entity_system.h"
#include "base/entity/shared/ntg_entity_vec.h"

ntg_entity* ntg_entity_create(struct ntg_entity_init_data init_data)
{
    ntg_entity* entity = (ntg_entity*)malloc(init_data.type->_size);
    assert(entity != NULL);
    memset(entity, 0, init_data.type->_size);

    entity->_type = init_data.type;
    entity->__deinit_fn = (init_data.deinit_fn != NULL) ?
        init_data.deinit_fn : _ntg_entity_deinit_fn;
    entity->__system = init_data.system;

    ntg_entity_system_register(init_data.system, entity);

    return entity;
}

void ntg_entity_destroy(ntg_entity* entity)
{
    assert(entity != NULL);

    entity->__deinit_fn(entity);
    
    free(entity);
}

void _ntg_entity_deinit_fn(ntg_entity* entity)
{
    assert(entity != NULL);
    
    ntg_entity_system_unregister(entity->__system, entity);

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
