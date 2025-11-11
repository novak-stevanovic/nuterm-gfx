#ifndef _NTG_EVENT_TYPES_H_
#define _NTG_EVENT_TYPES_H_

#include "shared/ntg_xy.h"

#define NTG_EVT_APP_RESIZE 1
struct ntg_evt_app_resize_data
{
    struct ntg_xy old, new;
};

#endif // _NTG_EVENT_TYPES_H_
