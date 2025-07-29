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

#define NTG_ETYPE_STAGE_RESIZE 10 // data type: struct ntg_size_change
#define NTG_ETYPE_STAGE_SCENE_CHANGE 11 // data type: struct ntg_scene_change
#define NTG_ETYPE_STAGE_RENDER 12 // data type: NONE
 
#define NTG_ETYPE_SCENE_RESIZE 100 // data type: struct ntg_size_change
#define NTG_ETYPE_SCENE_ROOT_CHANGE 102 // data type: struct ntg_object_change
#define NTG_ETYPE_SCENE_FOCUSED_CHANGE 103 // data type: struct ntg_object_change
#define NTG_ETYPE_SCENE_LAYOUT 104 // data type: NONE
#define NTG_ETYPE_SCENE_OBJECT_REGISTER 105 // data type: ntg_object*
#define NTG_ETYPE_SCENE_OBJECT_UNREGISTER 105 // data type: ntg_object*

/* Object's internals has been changed(for example, its text, color, borders etc.)
 * This event must be raised any time an object's content must be updated visually,
 * even if another event, one that is of a more specific type, is raised. */
#define NTG_ETYPE_OBJECT_INTERNALS_CHANGE 1000 // data type: NONE

/* Object's preferred size has been changed */
#define NTG_ETYPE_OBJECT_PREF_SIZE_CHANGE 1001 // data type: struct ntg_size_change

#endif // _NTG_EVENT_TYPES_H_
