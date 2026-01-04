#ifndef _NTG_ENTITY_MAP_H_
#define _NTG_ENTITY_MAP_H_

#include "core/entity/shared/ntg_event_obs_vec.h"
#include "shared/ntg_typedef.h"

struct ntg_entity_data
{
    unsigned int id;
    ntg_event_obs_vec observers;
};

ntg_entity_map* ntg_entity_map_new();
void ntg_entity_map_destroy(ntg_entity_map* map);

void ntg_entity_map_add(ntg_entity_map* map, ntg_entity* object, unsigned int id);
void ntg_entity_map_rm(ntg_entity_map* map, ntg_entity* entity);
struct ntg_entity_data* ntg_entity_map_get(ntg_entity_map* map, const ntg_entity* entity);

void ntg_entity_map_get_keys(const ntg_entity_map* map, ntg_entity_vec* out_vec);

#endif // _NTG_ENTITY_MAP_H_
