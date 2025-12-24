#ifndef __NTG_ENTITY_H__
#define __NTG_ENTITY_H__

#include <stdbool.h>
#include <stddef.h>

typedef struct ntg_entity_type ntg_entity_type;

struct ntg_entity_type
{
    const ntg_entity_type* __parent;
    size_t _size;
};

bool ntg_entity_instanceof(const ntg_entity_type* type, const ntg_entity_type* ancestor);
bool ntg_entity_is(const ntg_entity_type* type, const ntg_entity_type* ancestor);

extern const ntg_entity_type NTG_ENTITY_DEFAULT;

/* NTG_OBJECT */

extern const ntg_entity_type NTG_ENTITY_OBJECT, NTG_ENTITY_WIDGET,
       NTG_ENTITY_PROG_BAR, NTG_ENTITY_COLOR_BLOCK, NTG_ENTITY_LABEL,
       NTG_ENTITY_BOX, NTG_ENTITY_BORDER_BOX,
       NTG_ENTITY_DECORATOR, NTG_ENTITY_PADDING,
       NTG_ENTITY_DEF_PADDING, NTG_ENTITY_BORDER,
       NTG_ENTITY_DEF_BORDER;

/* NTG_SCENE */

extern const ntg_entity_type NTG_ENTITY_SCENE, NTG_ENTITY_DEF_SCENE;

/* NTG_STAGE */

extern const ntg_entity_type NTG_ENTITY_STAGE, NTG_ENTITY_DEF_STAGE;

/* NTG_RENDERER */

extern const ntg_entity_type NTG_ENTITY_RENDERER, NTG_ENTITY_DEF_RENDERER;

/* NTG_LOOP */

extern const ntg_entity_type NTG_ENTITY_LOOP;

/* NTG_TASKMASTER */

extern const ntg_entity_type NTG_ENTITY_TASKMASTER;

#endif // __NTG_ENTITY_H__
