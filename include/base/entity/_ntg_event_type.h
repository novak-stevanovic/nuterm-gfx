#ifndef __NTG_EVENT_TYPE_H__
#define __NTG_EVENT_TYPE_H__

#include "nt_event.h"
#include "shared/ntg_xy.h"

#define NTG_EVENT_APP_RESIZE 1
struct ntg_event_app_resize_data
{
    struct ntg_xy old, new;
};

#define NTG_EVENT_APP_KEYPRESS 2
struct ntg_event_app_key_data
{
    struct nt_key_event key;
};

#endif // __NTG_EVENT_TYPE_H__
