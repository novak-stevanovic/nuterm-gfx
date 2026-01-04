#ifndef __NTG_ENTITY_SYSTEM_H__
#define __NTG_ENTITY_SYSTEM_H__

#include "shared/ntg_typedef.h"

ntg_entity_system* _ntg_entity_system_new();
void _ntg_entity_system_destroy(ntg_entity_system* system);

void _ntg_entity_system_register(ntg_entity_system* system, ntg_entity* entity);
void _ntg_entity_system_unregister(ntg_entity_system* system, ntg_entity* entity);
unsigned int _ntg_entity_system_get_id(ntg_entity_system* system, const ntg_entity* entity);

void _ntg_entity_system_raise_event(
        ntg_entity_system* system,
        ntg_entity* source, 
        ntg_entity* target,
        unsigned int type, void* data);
void _ntg_entity_system_add_observe(
        ntg_entity_system* system,
        ntg_entity* observer,
        ntg_entity* observed,
        ntg_event_handler_fn handler_fn);
void _ntg_entity_system_rm_observe(
        ntg_entity_system* system,
        ntg_entity* observer,
        ntg_entity* observed,
        ntg_event_handler_fn handler_fn);
bool _ntg_entity_system_has_observe(
        ntg_entity_system* system,
        ntg_entity* observer,
        ntg_entity* observed,
        ntg_event_handler_fn handler_fn);

#endif // __NTG_ENTITY_SYSTEM_H__
