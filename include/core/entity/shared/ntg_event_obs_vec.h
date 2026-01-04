#ifndef _NTG_EVENT_OBS_VEC_H_
#define _NTG_EVENT_OBS_VEC_H_

#include <stddef.h>
#include "shared/ntg_typedef.h"

// event observe
struct ntg_event_obs
{
    ntg_entity* observer;
    ntg_event_handler_fn handler;
};

struct ntg_event_observe_vec
{
    struct ntg_event_obs* _data;
    size_t _count;
    size_t __capacity;
    size_t __data_size;
};

void ntg_event_obs_vec_init(ntg_event_obs_vec* vec);
void ntg_event_obs_vec_deinit(ntg_event_obs_vec* vec);

void ntg_event_obs_vec_add(ntg_event_obs_vec* vec, struct ntg_event_obs obs);
void ntg_event_obs_vec_ins(ntg_event_obs_vec* vec, struct ntg_event_obs obs, size_t pos);
void ntg_event_obs_vec_rm(ntg_event_obs_vec* vec, struct ntg_event_obs obs);

/* Returns SIZE_MAX on fail/non-existing. */
size_t ntg_event_obs_vec_find(const ntg_event_obs_vec* vec, struct ntg_event_obs obs);
bool ntg_event_obs_vec_has(const ntg_event_obs_vec* vec, struct ntg_event_obs obs);

#endif // _NTG_EVENT_OBS_VEC_H_
