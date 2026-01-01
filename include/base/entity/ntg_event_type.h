#ifndef _NTG_EVENT_TYPE_H_
#define _NTG_EVENT_TYPE_H_

/* Included in ntg_entity.h */

#include "base/ntg_cell.h"
#include "nt_event.h"
#include "shared/ntg_xy.h"

typedef struct ntg_object ntg_object;

#define NTG_EVENT_LOOP_RSZ 1
struct ntg_event_loop_rsz_data
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
    ntg_object* child; // widget
};

#define NTG_EVENT_OBJECT_CHLDRM 4
struct ntg_event_object_chldrm_data
{
    ntg_object* child; // widget
};

/* Raised any time a change occurs in the object tree, including
 * decorator child add/remove */
#define NTG_EVENT_OBJECT_CHLDADD_DCR 5
struct ntg_event_object_chldadd_dcr_data
{
    ntg_object* child;
};

#define NTG_EVENT_OBJECT_CHLDRM_DCR 6
struct ntg_event_object_chldrm_dcr_data
{
    ntg_object* child;
};

#define NTG_EVENT_OBJECT_REMEASURE 7

#define NTG_EVENT_OBJECT_REDRAW 8

#define NTG_EVENT_SCENE_OBJADD 9
struct ntg_event_scene_objadd_data
{
    ntg_object* object;
};

#define NTG_EVENT_SCENE_OBJRM 10
struct ntg_event_scene_objrm_data
{
    ntg_object* object;
};

#endif // _NTG_EVENT_TYPE_H_
