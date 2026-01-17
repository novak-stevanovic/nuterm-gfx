#ifndef NTG_EVENT_TYPE_H
#define NTG_EVENT_TYPE_H

#include "shared/ntg_typedef.h"
#include "nt_event.h"

/* -------------------------------------------------------------------------- */
/* OBJECT */
/* -------------------------------------------------------------------------- */

#define NTG_EVENT_OBJECT_CHLDADD 4
struct ntg_event_object_chldadd_data
{
    ntg_object* child; // widget
};

#define NTG_EVENT_OBJECT_CHLDRM 5
struct ntg_event_object_chldrm_data
{
    ntg_object* child; // widget
};

/* Raised any time a change occurs in the object tree, including
 * decorator child add/rm */
#define NTG_EVENT_OBJECT_CHLDADD_DCR 6
struct ntg_event_object_chldadd_dcr_data
{
    ntg_object* child;
};

#define NTG_EVENT_OBJECT_CHLDRM_DCR 7
struct ntg_event_object_chldrm_dcr_data
{
    ntg_object* child;
};

#define NTG_EVENT_OBJECT_DIFF 8

#define NTG_EVENT_OBJECT_PADCHNG 13
struct ntg_event_object_padchng
{
    ntg_padding *old, *new;
};

#define NTG_EVENT_OBJECT_BORDCHNG 14
struct ntg_event_object_bordchng
{
    ntg_padding *old, *new;
};

/* -------------------------------------------------------------------------- */
/* SCENE */
/* -------------------------------------------------------------------------- */

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

#define NTG_EVENT_SCENE_DIFF 11

/* -------------------------------------------------------------------------- */
/* STAGE */
/* -------------------------------------------------------------------------- */

#define NTG_EVENT_STAGE_SCNCHNG 12
struct ntg_event_stage_scnchng_data
{
    ntg_scene *old, *new;
};

/* -------------------------------------------------------------------------- */
/* LOOP*/
/* -------------------------------------------------------------------------- */

#define NTG_EVENT_LOOP_EVENT 1
struct ntg_event_loop_event_data
{
    struct nt_event event;
};

/* -------------------------------------------------------------------------- */
/* RENDERER*/
/* -------------------------------------------------------------------------- */

#endif // NTG_EVENT_TYPE_H
