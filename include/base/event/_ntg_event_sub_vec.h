#ifndef __NTG_EVENT_SUB_VEC_H__
#define __NTG_EVENT_SUB_VEC_H__

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>
#include "base/event/ntg_event.h"

struct ntg_event_sub
{
    void* subscriber;
    ntg_event_handler handler;
};

typedef struct ntg_event_sub_vec
{
    struct ntg_event_sub* _data;
    const size_t _count;
    size_t __capacity;
    size_t __data_size;
} ntg_event_sub_vec;

void __ntg_event_sub_vec_init__(ntg_event_sub_vec* vec);
void __ntg_event_sub_vec_deinit__(ntg_event_sub_vec* vec);

ntg_event_sub_vec* ntg_event_sub_vec_new();
void ntg_event_sub_vec_destroy(ntg_event_sub_vec* vec);

void ntg_event_sub_vec_append(
        ntg_event_sub_vec* vec,
        void* subscriber,
        ntg_event_handler handler);
void ntg_event_sub_vec_remove(ntg_event_sub_vec* vec,
        void* subscriber,
        ntg_event_handler handler);

/* Returns SIZE_MAX on fail/non-existing. */
size_t ntg_event_sub_vec_find(
        const ntg_event_sub_vec* vec,
        const void* subscriber);
size_t ntg_event_sub_vec_find_(
        const ntg_event_sub_vec* vec,
        const void* subscriber,
        ntg_event_handler handler);

bool ntg_event_sub_vec_contains(
        const ntg_event_sub_vec* vec,
        const void* subscriber);
bool ntg_event_sub_vec_contains_(
        const ntg_event_sub_vec* vec,
        const void* subscriber,
        ntg_event_handler handler);

#endif // __NTG_EVENT_SUB_VEC_H__
