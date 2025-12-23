#ifndef _NTG_ENTITY_H_
#define _NTG_ENTITY_H_

#include <stdbool.h>
#include "base/entity/_ntg_entity_type.h"
#include "base/entity/_ntg_event_type.h"

/* -------------------------------------------------------------------------- */
/* DECLARATIONS */
/* -------------------------------------------------------------------------- */

typedef struct ntg_entity ntg_entity;
typedef struct ntg_entity_group ntg_entity_group;
typedef struct ntg_entity_register ntg_entity_register;
typedef struct ntg_entity_system ntg_entity_system;
typedef struct sarena sarena;
struct ntg_event;

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

typedef void (*ntg_entity_deinit_fn)(ntg_entity* entity);

typedef void (*ntg_event_handler_fn)(
        ntg_entity* observer,
        struct ntg_event event);

struct ntg_entity
{
    const ntg_entity_type* type;
    sarena* _arena;

    ntg_entity_deinit_fn __deinit_fn;
    ntg_entity_system* __system;
};

struct ntg_event
{
    unsigned int id;
    unsigned int type;
    ntg_entity* source;
    void* data;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* ENTITY */
/* ------------------------------------------------------ */

struct ntg_entity_init_data
{
    const ntg_entity_type* type;
    ntg_entity_deinit_fn deinit_fn;
    ntg_entity_group* group;
    ntg_entity_system* system;
};

void _ntg_entity_init_(ntg_entity* entity, struct ntg_entity_init_data init_data);
void _ntg_entity_deinit_(ntg_entity* entity);

void _ntg_entity_deinit_fn(ntg_entity* entity);

/* ------------------------------------------------------ */

void ntg_entity_raise_event(
        ntg_entity* entity,
        unsigned int type,
        void* data);
void ntg_entity_observe(
        ntg_entity* observer,
        ntg_entity* observed,
        ntg_event_handler_fn handler_fn);
void ntg_entity_stop_observing(
        ntg_entity* observer,
        ntg_entity* observed,
        ntg_event_handler_fn handler_fn);
bool ntg_entity_is_observing(
        ntg_entity* observer,
        ntg_entity* observed,
        ntg_event_handler_fn handler_fn);

/* ------------------------------------------------------ */
/* ENTITY GROUP */
/* ------------------------------------------------------ */

ntg_entity_group* ntg_entity_group_new(sarena* arena);
void ntg_entity_group_destroy(ntg_entity_group* group);
sarena* ntg_entity_group_get_arena(ntg_entity_group* group);

#endif // _NTG_ENTITY_H_
