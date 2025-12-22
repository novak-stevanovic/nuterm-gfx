#ifndef __NTG_ENTITY_MAP_H__
#define __NTG_ENTITY_MAP_H__

#include <stdbool.h>

typedef struct ntg_entity_map ntg_entity_map;
typedef struct ntg_entity ntg_entity;
typedef struct ntg_event_sub_vec ntg_event_sub_vec;

struct ntg_entity_data
{
    bool deinit;
    unsigned int id;
    ntg_event_sub_vec* subs;
};

ntg_entity_map* ntg_entity_map_new();
void ntg_entity_map_destroy(ntg_entity_map* map);

void ntg_entity_map_add(ntg_entity_map* map, ntg_entity* entity, unsigned int id);
void ntg_entity_map_remove(ntg_entity_map* map, ntg_entity* entity);
struct ntg_entity_data* ntg_entity_map_get(ntg_entity_map* map, const ntg_entity* entity);

#endif // __NTG_ENTITY_MAP_H__
