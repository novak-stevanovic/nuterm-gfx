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
    ntg_object* child;
};

#define NTG_EVENT_OBJECT_CHLDRM 5
struct ntg_event_object_chldrm_data
{
    ntg_object* child;
};

#define NTG_EVENT_OBJECT_PRNTCHNG 6
struct ntg_event_object_prntchng_data
{
    ntg_object *old, *new;
};

#define NTG_EVENT_WIDGET_CHLDADD 100
struct ntg_event_widget_chldadd_data
{
    ntg_widget* child;
};

#define NTG_EVENT_WIDGET_CHLDRM 101
struct ntg_event_widget_chldrm_data
{
    ntg_widget* child;
};

#define NTG_EVENT_WIDGET_PRNTCHNG 102
struct ntg_event_widget_prntchng_data
{
    ntg_widget *old, *new;
};

#define NTG_EVENT_WIDGET_BGCHNG 103
struct ntg_event_widget_bgchng_data
{
    struct ntg_vcell old, new;
};

#define NTG_EVENT_WIDGET_PADCHNG 104
struct ntg_event_widget_padchng_data
{
    ntg_decorator *old, *new;
};

#define NTG_EVENT_WIDGET_BORDCHNG 105
struct ntg_event_widget_bordchng_data
{
    ntg_decorator *old, *new;
};

#define NTG_EVENT_OBJECT_SCNCHNG 7
struct ntg_event_object_scnchng_data
{
    ntg_scene *old, *new;
};

#define NTG_EVENT_OBJECT_DIFF 8

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

#define NTG_EVENT_SCENE_ROOTCHNG 12
struct ntg_event_scene_rootchng_data
{
    ntg_widget *old, *new;
};

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
