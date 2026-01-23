#ifndef NTG_EVENT_TYPE_H
#define NTG_EVENT_TYPE_H

#include "shared/ntg_typedef.h"
#include "nt_event.h"

/* -------------------------------------------------------------------------- */
/* OBJECT */
/* -------------------------------------------------------------------------- */

#define NTG_EVENT_OBJECT_CHLDADD 1
struct ntg_event_object_chldadd_data
{
    ntg_object* child;
};

#define NTG_EVENT_OBJECT_CHLDRM 2
struct ntg_event_object_chldrm_data
{
    ntg_object* child;
};

#define NTG_EVENT_OBJECT_PRNTCHNG 3
struct ntg_event_object_prntchng_data
{
    ntg_object *old, *new;
};

#define NTG_EVENT_OBJECT_SCNCHNG 4
struct ntg_event_object_scnchng_data
{
    ntg_scene *old, *new;
};

#define NTG_EVENT_OBJECT_DIFF 5

#define NTG_EVENT_WIDGET_CHLDADD 6
struct ntg_event_widget_chldadd_data
{
    ntg_widget* child;
};

#define NTG_EVENT_WIDGET_CHLDRM 7
struct ntg_event_widget_chldrm_data
{
    ntg_widget* child;
};

#define NTG_EVENT_WIDGET_PRNTCHNG 8
struct ntg_event_widget_prntchng_data
{
    ntg_widget *old, *new;
};

#define NTG_EVENT_WIDGET_BGCHNG 9
struct ntg_event_widget_bgchng_data
{
    struct ntg_vcell old, new;
};

#define NTG_EVENT_WIDGET_PADCHNG 10
struct ntg_event_widget_padchng_data
{
    ntg_decorator *old, *new;
};

#define NTG_EVENT_WIDGET_BORDCHNG 11
struct ntg_event_widget_bordchng_data
{
    ntg_decorator *old, *new;
};

/* -------------------------------------------------------------------------- */
/* SCENE */
/* -------------------------------------------------------------------------- */

#define NTG_EVENT_SCENE_OBJADD 100
struct ntg_event_scene_objadd_data
{
    ntg_object* object;
};

#define NTG_EVENT_SCENE_OBJRM 101
struct ntg_event_scene_objrm_data
{
    ntg_object* object;
};

#define NTG_EVENT_SCENE_ROOTCHNG 103
struct ntg_event_scene_rootchng_data
{
    ntg_widget *old, *new;
};

/* -------------------------------------------------------------------------- */
/* STAGE */
/* -------------------------------------------------------------------------- */

#define NTG_EVENT_STAGE_SCNCHNG 200
struct ntg_event_stage_scnchng_data
{
    ntg_scene *old, *new;
};

/* -------------------------------------------------------------------------- */
/* LOOP */
/* -------------------------------------------------------------------------- */

#define NTG_EVENT_LOOP_GENERIC 300
struct ntg_event_loop_generic_data
{
    struct nt_event event;
};

#define NTG_EVENT_LOOP_KEY 301
struct ntg_event_loop_key_data
{
    struct nt_key_event key;
};

#define NTG_EVENT_LOOP_MOUSE 302
struct ntg_event_loop_mouse_data
{
    struct nt_mouse_event mouse;
};

/* -------------------------------------------------------------------------- */
/* RENDERER */
/* -------------------------------------------------------------------------- */

#endif // NTG_EVENT_TYPE_H
