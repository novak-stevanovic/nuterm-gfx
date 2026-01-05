#include "core/loop/_ntg_loop.h"
#include "core/entity/ntg_entity_type.h"
#include <assert.h>

ntg_loop* _ntg_loop_new(ntg_entity_system* system)
{
    struct ntg_entity_init_data entity_data = {
        .type = &NTG_ENTITY_LOOP,
        .deinit_fn = _ntg_loop_deinit_fn,
        .system = system
    };

    ntg_loop* new = (ntg_loop*)ntg_entity_create(entity_data);
    assert(new != NULL);

    return new;
}

void _ntg_loop_init(ntg_loop* loop)
{
    assert(loop != NULL);
}

void _ntg_loop_deinit_fn(ntg_entity* entity)
{
}
