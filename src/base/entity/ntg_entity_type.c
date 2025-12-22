#include "base/entity/ntg_entity_type.h"
#include <stdlib.h>

bool ntg_entity_instanceof(const ntg_entity_type* type, const ntg_entity_type* ancestor)
{
    const ntg_entity_type* it = type;

    while (it != NULL)
    {
        if (it == ancestor) return true;

        it = it->__parent;
    }

    return false;
}

/* -------------------------------------------------------------------------- */
/* Base                                                                        */
/* -------------------------------------------------------------------------- */

const ntg_entity_type NTG_ENTITY_TYPE_DEFAULT = {
    .__parent = NULL
};

/* -------------------------------------------------------------------------- */
/* NTG_OBJECT hierarchy                                                       */
/* -------------------------------------------------------------------------- */

const ntg_entity_type NTG_ENTITY_TYPE_OBJECT = {
    .__parent = &NTG_ENTITY_TYPE_DEFAULT
};

const ntg_entity_type NTG_ENTITY_TYPE_WIDGET = {
    .__parent = &NTG_ENTITY_TYPE_OBJECT
};

const ntg_entity_type NTG_ENTITY_TYPE_BOX = {
    .__parent = &NTG_ENTITY_TYPE_WIDGET
};

const ntg_entity_type NTG_ENTITY_TYPE_BORDER_BOX = {
    .__parent = &NTG_ENTITY_TYPE_BOX
};

const ntg_entity_type NTG_ENTITY_TYPE_LABEL = {
    .__parent = &NTG_ENTITY_TYPE_WIDGET
};

const ntg_entity_type NTG_ENTITY_TYPE_COLOR_BLOCK = {
    .__parent = &NTG_ENTITY_TYPE_WIDGET
};

const ntg_entity_type NTG_ENTITY_TYPE_PROG_BAR = {
    .__parent = &NTG_ENTITY_TYPE_WIDGET
};

const ntg_entity_type NTG_ENTITY_TYPE_DECORATOR = {
    .__parent = &NTG_ENTITY_TYPE_WIDGET
};

const ntg_entity_type NTG_ENTITY_TYPE_PADDING = {
    .__parent = &NTG_ENTITY_TYPE_DECORATOR
};

const ntg_entity_type NTG_ENTITY_TYPE_DEF_PADDING = {
    .__parent = &NTG_ENTITY_TYPE_PADDING
};

const ntg_entity_type NTG_ENTITY_TYPE_BORDER = {
    .__parent = &NTG_ENTITY_TYPE_DECORATOR
};

const ntg_entity_type NTG_ENTITY_TYPE_DEF_BORDER = {
    .__parent = &NTG_ENTITY_TYPE_BORDER
};

/* -------------------------------------------------------------------------- */
/* NTG_SCENE                                                                  */
/* -------------------------------------------------------------------------- */

const ntg_entity_type NTG_ENTITY_TYPE_SCENE = {
    .__parent = &NTG_ENTITY_TYPE_DEFAULT
};

const ntg_entity_type NTG_ENTITY_TYPE_DEF_SCENE = {
    .__parent = &NTG_ENTITY_TYPE_SCENE
};

/* -------------------------------------------------------------------------- */
/* NTG_STAGE                                                                  */
/* -------------------------------------------------------------------------- */

const ntg_entity_type NTG_ENTITY_TYPE_STAGE = {
    .__parent = &NTG_ENTITY_TYPE_DEFAULT
};

const ntg_entity_type NTG_ENTITY_TYPE_DEF_STAGE = {
    .__parent = &NTG_ENTITY_TYPE_STAGE
};

/* -------------------------------------------------------------------------- */
/* NTG_RENDERER                                                               */
/* -------------------------------------------------------------------------- */

const ntg_entity_type NTG_ENTITY_TYPE_RENDERER = {
    .__parent = &NTG_ENTITY_TYPE_DEFAULT
};

const ntg_entity_type NTG_ENTITY_TYPE_DEF_RENDERER = {
    .__parent = &NTG_ENTITY_TYPE_RENDERER
};

/* -------------------------------------------------------------------------- */
/* NTG_LOOP                                                                   */
/* -------------------------------------------------------------------------- */

const ntg_entity_type NTG_ENTITY_TYPE_LOOP = {
    .__parent = &NTG_ENTITY_TYPE_DEFAULT
};
