#ifndef _NTG_ENTITY_H_
#define _NTG_ENTITY_H_

#include "shared/ntg_typedef.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

/* Base class for GUI objects. Entities must be dynamically allocated. */

struct ntg_entity
{
    const ntg_entity_type* _type;

    ntg_entity_deinit_fn __deinit_fn;
    ntg_entity_system* __system;
};

// Extensible
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

struct ntg_entity_init_data
{
    const ntg_entity_type* type;
    ntg_entity_deinit_fn deinit_fn;
    ntg_entity_system* system;
};

ntg_entity* ntg_entity_create(struct ntg_entity_init_data init_data);
void ntg_entity_destroy(ntg_entity* entity);

/* ------------------------------------------------------ */

void ntg_entity_raise_event(
        ntg_entity* entity,
        ntg_entity* target,
        unsigned int type, void* data);

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

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

#endif // _NTG_ENTITY_H_
