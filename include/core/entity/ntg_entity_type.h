#ifndef NTG_ENTITY_TYPE_H
#define NTG_ENTITY_TYPE_H

#include <stdbool.h>
#include <stddef.h>
#include "shared/ntg_typedef.h"

struct ntg_entity_type
{
    const ntg_entity_type* __parent;
    size_t _size;
};

bool ntg_entity_instance_of(const ntg_entity_type* type,
                            const ntg_entity_type* ancestor);
bool ntg_entity_is_same_type(const ntg_entity_type* type,
                             const ntg_entity_type* ancestor);

extern const ntg_entity_type NTG_ENTITY;

/* NTG_OBJECT */

extern const ntg_entity_type NTG_ENTITY_OBJECT, NTG_ENTITY_WIDGET,
       NTG_ENTITY_PROG_BAR, NTG_ENTITY_COLOR_BLOCK, NTG_ENTITY_LABEL,
       NTG_ENTITY_BOX, NTG_ENTITY_BORDER_BOX,
       NTG_ENTITY_DECORATOR, NTG_ENTITY_PADDING,
       NTG_ENTITY_DEF_PADDING, NTG_ENTITY_BORDER,
       NTG_ENTITY_DEF_BORDER, NTG_ENTITY_FOCUS_MGR;

/* NTG_SCENE */

extern const ntg_entity_type NTG_ENTITY_SCENE, NTG_ENTITY_DEF_SCENE;

/* NTG_FOCUSER */

extern const ntg_entity_type NTG_ENTITY_FOCUSER, NTG_ENTITY_DEF_FOCUSER;

/* NTG_STAGE */

extern const ntg_entity_type NTG_ENTITY_STAGE, NTG_ENTITY_DEF_STAGE;

/* NTG_RENDERER */

extern const ntg_entity_type NTG_ENTITY_RENDERER, NTG_ENTITY_DEF_RENDERER;

/* NTG_LOOP */

extern const ntg_entity_type NTG_ENTITY_LOOP;

#endif // NTG_ENTITY_TYPE_H
