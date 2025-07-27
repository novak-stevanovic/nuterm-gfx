#ifndef _NTG_EVENT_TYPES_H_
#define _NTG_EVENT_TYPES_H_

#include "shared/ntg_xy.h"

typedef struct ntg_object ntg_object;
typedef struct ntg_scene ntg_scene;

struct ntg_size_change
{
    struct ntg_xy old, new;
};

struct ntg_scene_change
{
    ntg_scene *old, *new;
};

struct ntg_object_change
{
    ntg_object *old, *new;
};

#define NTG_STAGE_RESIZE 10
#define NTG_STAGE_SCENE_CHANGE 11
#define NTG_STAGE_RENDER 12

#define NTG_SCENE_RESIZE 100
#define NTG_SCENE_ROOT_CHANGE 102
#define NTG_SCENE_FOCUSED_CHANGE 103
#define NTG_SCENE_LAYOUT 104
#define NTG_SCENE_OBJECT_REGISTER 105
#define NTG_SCENE_OBJECT_UNREGISTER 105

/* Object's internal's has been changed(for example, its text, color, borders etc.) */
#define NTG_OBJECT_INTERNALS_CHANGE 1000

/* Object's preferred size has been changed */
#define NTG_OBJECT_PREF_SIZE_CHANGE 1001

#endif // _NTG_EVENT_TYPES_H_
