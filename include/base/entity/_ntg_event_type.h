#ifndef __NTG_EVENT_H__
#define __NTG_EVENT_H__

#include "base/ntg_cell.h"
#include "nt_event.h"
#include "shared/ntg_xy.h"

typedef struct ntg_object ntg_object;

#define NTG_EVENT_LOOP_RSZ 1
struct ntg_event_loop_resize_data
{
    struct ntg_xy old, new;
};

#define NTG_EVENT_LOOP_KEY 2
struct ntg_event_loop_key_data
{
    struct nt_key_event key;
};

#define NTG_EVENT_OBJECT_CHLDADD 3
struct ntg_event_object_chldadd_data
{
    ntg_object* child;
};

#define NTG_EVENT_OBJECT_CHLDRM 4
struct ntg_event_object_chldrm_data
{
    ntg_object* child;
};

// #define NTG_EVENT_OBJECT_BRDRCHNG 4
// struct ntg_event_object_brdrchng_data
// {
// };
//
// #define NTG_EVENT_OBJECT_PADCHNG 5
// struct ntg_event_object_padchng_data
// {
// };

#endif // __NTG_EVENT_H__
