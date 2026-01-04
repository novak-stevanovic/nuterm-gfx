#ifndef __NTG_EVENT_SUB_VEC_H__
#define __NTG_EVENT_SUB_VEC_H__

#include <stddef.h>
#include "shared/ntg_typedef.h"

struct ntg_event_sub
{
    ntg_entity* entity;
    ntg_event_handler_fn handler;
};

struct ntg_event_sub_vec
{
    struct ntg_event_sub* _data;
    size_t _count;
    size_t __capacity;
    size_t __data_size;
};

void _ntg_event_sub_vec_init_(ntg_event_sub_vec* vec);
void _ntg_event_sub_vec_deinit_(ntg_event_sub_vec* vec);

ntg_event_sub_vec* ntg_event_sub_vec_new();
void ntg_event_sub_vec_destroy(ntg_event_sub_vec* vec);

void ntg_event_sub_vec_append(
        ntg_event_sub_vec* vec,
        ntg_entity* observer,
        ntg_event_handler_fn handler);
void ntg_event_sub_vec_insert(
        ntg_event_sub_vec* vec,
        ntg_entity* observer,
        ntg_event_handler_fn handler,
        size_t pos);
void ntg_event_sub_vec_remove(
        ntg_event_sub_vec* vec,
        ntg_entity* observer,
        ntg_event_handler_fn handler);

/* Returns SIZE_MAX on fail/non-existing. */
size_t ntg_event_sub_vec_find_(
        const ntg_event_sub_vec* vec,
        const ntg_entity* observer,
        ntg_event_handler_fn handler);

bool ntg_event_sub_vec_contains(
        const ntg_event_sub_vec* vec,
        const ntg_entity* observer,
        ntg_event_handler_fn handler);

#endif // __NTG_EVENT_SUB_VEC_H__
