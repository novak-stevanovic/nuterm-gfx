#ifndef __NTG_ENTITY_TYPE_H__
#define __NTG_ENTITY_TYPE_H__

#include <stdbool.h>

typedef struct ntg_entity_type ntg_entity_type;

struct ntg_entity_type
{
    const ntg_entity_type* __parent;
};

bool ntg_entity_instanceof(const ntg_entity_type* type, const ntg_entity_type* ancestor);

extern const ntg_entity_type NTG_ENTITY_TYPE_DEFAULT;

/* NTG_OBJECT */

extern const ntg_entity_type NTG_ENTITY_TYPE_OBJECT, NTG_ENTITY_TYPE_WIDGET,
       NTG_ENTITY_TYPE_PROG_BAR, NTG_ENTITY_TYPE_COLOR_BLOCK, NTG_ENTITY_TYPE_LABEL,
       NTG_ENTITY_TYPE_BOX, NTG_ENTITY_TYPE_BORDER_BOX,
       NTG_ENTITY_TYPE_DECORATOR, NTG_ENTITY_TYPE_PADDING,
       NTG_ENTITY_TYPE_DEF_PADDING, NTG_ENTITY_TYPE_BORDER,
       NTG_ENTITY_TYPE_DEF_BORDER;

/* NTG_SCENE */

extern const ntg_entity_type NTG_ENTITY_TYPE_SCENE, NTG_ENTITY_TYPE_DEF_SCENE;

/* NTG_STAGE */

extern const ntg_entity_type NTG_ENTITY_TYPE_STAGE, NTG_ENTITY_TYPE_DEF_STAGE;

/* NTG_RENDERER */

extern const ntg_entity_type NTG_ENTITY_TYPE_RENDERER, NTG_ENTITY_TYPE_DEF_RENDERER;

/* NTG_LOOP */

extern const ntg_entity_type NTG_ENTITY_TYPE_LOOP;

/* NTG_TASKMASTER */

extern const ntg_entity_type NTG_ENTITY_TYPE_TASKMASTER;

#endif // __NTG_ENTITY_TYPE_H__
