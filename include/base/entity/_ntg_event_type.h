#ifndef __NTG_EVENT_H__
#define __NTG_EVENT_H__

#include "base/ntg_cell.h"
#include "nt_event.h"
#include "shared/ntg_xy.h"

#define NTG_EVENT_LOOPRSZ 1
struct ntg_event_loop_resize_data
{
    struct ntg_xy old, new;
};

#define NTG_EVENT_LOOPKEYPRS 2
struct ntg_event_loop_key_data
{
    struct nt_key_event key;
};

#endif // __NTG_EVENT_H__
