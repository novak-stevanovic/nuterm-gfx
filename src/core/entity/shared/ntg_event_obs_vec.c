#include "ntg.h"
#include "shared/ntg_vec.h"
#include "core/entity/shared/ntg_event_obs_vec.h"
#include <assert.h>
#include <stdlib.h>

struct _ntg_event_obs
{
    const ntg_entity* entity;
    ntg_event_handler_fn handler;
};

void ntg_event_obs_vec_init(ntg_event_obs_vec* vec)
{
    assert(vec != NULL);

    ntg_vec_init((ntg_vec*)vec, sizeof(struct ntg_event_obs), 5, 2);
}

void ntg_event_obs_vec_deinit(ntg_event_obs_vec* vec)
{
    assert(vec != NULL);

    ntg_vec_deinit((ntg_vec*)vec);
}

void ntg_event_obs_vec_add(ntg_event_obs_vec* vec, struct ntg_event_obs obs)
{
    assert(vec != NULL);
    assert(obs.observer != NULL);
    assert(obs.handler != NULL);

    ntg_vec_add((ntg_vec*)vec, &obs, sizeof(struct ntg_event_obs));
}

void ntg_event_obs_vec_ins(ntg_event_obs_vec* vec, struct ntg_event_obs obs, size_t pos)
{
    assert(vec != NULL);
    assert(obs.observer != NULL);
    assert(obs.handler != NULL);
    assert(pos <= vec->_count);

    ntg_vec_ins((ntg_vec*)vec, pos, &obs, sizeof(struct ntg_event_obs));
}

void ntg_event_obs_vec_rm(ntg_event_obs_vec* vec, struct ntg_event_obs obs)
{
    assert(vec != NULL);
    assert(obs.observer != NULL);
    assert(obs.handler != NULL);

    ntg_vec_rm((ntg_vec*)vec, &obs, NULL, sizeof(struct ntg_event_obs));
}

size_t ntg_event_obs_vec_find(const ntg_event_obs_vec* vec, struct ntg_event_obs obs)
{
    assert(vec != NULL);
    assert(obs.observer != NULL);
    assert(obs.handler != NULL);

    return ntg_vec_find((ntg_vec*)vec, &obs, NULL, sizeof(struct ntg_event_obs));
}

bool ntg_event_obs_vec_has(const ntg_event_obs_vec* vec, struct ntg_event_obs obs)
{
    assert(vec != NULL);
    assert(obs.observer != NULL);
    assert(obs.handler != NULL);

    return ntg_vec_has((ntg_vec*)vec, &obs, NULL, sizeof(struct ntg_event_obs));
}
