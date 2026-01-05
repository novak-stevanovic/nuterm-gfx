#include "core/loop/_ntg_loop.h"
#include "ntg.h"

bool ntg_entity_instance_of(const ntg_entity_type* type, const ntg_entity_type* ancestor)

{
    const ntg_entity_type* it = type;

    while (it != NULL)
    {
        if (it == ancestor) return true;

        it = it->__parent;
    }

    return false;
}

bool ntg_entity_is_same_type(const ntg_entity_type* type, const ntg_entity_type* ancestor)
{
    return (type == ancestor);
}

/* -------------------------------------------------------------------------- */
/* Base */
/* -------------------------------------------------------------------------- */

const ntg_entity_type NTG_ENTITY = {
    .__parent = NULL,
    ._size = sizeof(ntg_entity)
};

/* -------------------------------------------------------------------------- */
/* NTG_OBJECT */
/* -------------------------------------------------------------------------- */

const ntg_entity_type NTG_ENTITY_OBJECT = {
    .__parent = &NTG_ENTITY,
    ._size = sizeof(ntg_object)
};

const ntg_entity_type NTG_ENTITY_WIDGET = {
    .__parent = &NTG_ENTITY_OBJECT,
    ._size = sizeof(ntg_object)
};

const ntg_entity_type NTG_ENTITY_BOX = {
    .__parent = &NTG_ENTITY_WIDGET,
    ._size = sizeof(ntg_box)
};

const ntg_entity_type NTG_ENTITY_BORDER_BOX = {
    .__parent = &NTG_ENTITY_WIDGET,
    ._size = sizeof(ntg_border_box)
};

const ntg_entity_type NTG_ENTITY_LABEL = {
    .__parent = &NTG_ENTITY_WIDGET,
    ._size = sizeof(ntg_label)
};

const ntg_entity_type NTG_ENTITY_COLOR_BLOCK = {
    .__parent = &NTG_ENTITY_WIDGET,
    ._size = sizeof(ntg_color_block)
};

const ntg_entity_type NTG_ENTITY_PROG_BAR = {
    .__parent = &NTG_ENTITY_WIDGET,
    ._size = sizeof(ntg_prog_bar)
};

const ntg_entity_type NTG_ENTITY_DECORATOR = {
    .__parent = &NTG_ENTITY_OBJECT,
    ._size = sizeof(ntg_object)
};

const ntg_entity_type NTG_ENTITY_PADDING = {
    .__parent = &NTG_ENTITY_DECORATOR,
    ._size = sizeof(ntg_padding)
};

const ntg_entity_type NTG_ENTITY_DEF_PADDING = {
    .__parent = &NTG_ENTITY_PADDING,
    ._size = sizeof(ntg_def_padding)
};

const ntg_entity_type NTG_ENTITY_BORDER = {
    .__parent = &NTG_ENTITY_DECORATOR,
    ._size = sizeof(ntg_padding)
};

const ntg_entity_type NTG_ENTITY_DEF_BORDER = {
    .__parent = &NTG_ENTITY_BORDER,
    ._size = sizeof(ntg_def_border)
};

/* -------------------------------------------------------------------------- */
/* NTG_SCENE */
/* -------------------------------------------------------------------------- */

const ntg_entity_type NTG_ENTITY_SCENE = {
    .__parent = &NTG_ENTITY,
    ._size = sizeof(ntg_scene)
};

const ntg_entity_type NTG_ENTITY_DEF_SCENE = {
    .__parent = &NTG_ENTITY_SCENE,
    ._size = sizeof(ntg_def_scene)
};

/* -------------------------------------------------------------------------- */
/* NTG_FOCUSER */
/* -------------------------------------------------------------------------- */

const ntg_entity_type NTG_ENTITY_FOCUSER = {
    .__parent = &NTG_ENTITY,
    ._size = sizeof(ntg_focuser)
};

const ntg_entity_type NTG_ENTITY_DEF_FOCUSER = {
    .__parent = &NTG_ENTITY_FOCUSER,
    ._size = sizeof(ntg_single_focuser)
};

/* -------------------------------------------------------------------------- */
/* NTG_STAGE */
/* -------------------------------------------------------------------------- */

const ntg_entity_type NTG_ENTITY_STAGE = {
    .__parent = &NTG_ENTITY,
    ._size = sizeof(ntg_stage)
};

const ntg_entity_type NTG_ENTITY_DEF_STAGE = {
    .__parent = &NTG_ENTITY_STAGE,
    ._size = sizeof(ntg_def_stage)
};

/* -------------------------------------------------------------------------- */
/* NTG_RENDERER */
/* -------------------------------------------------------------------------- */

const ntg_entity_type NTG_ENTITY_RENDERER = {
    .__parent = &NTG_ENTITY,
    ._size = sizeof(ntg_renderer)
};

const ntg_entity_type NTG_ENTITY_DEF_RENDERER = {
    .__parent = &NTG_ENTITY_RENDERER,
    ._size = sizeof(ntg_def_renderer)
};

/* -------------------------------------------------------------------------- */
/* NTG_LOOP */
/* -------------------------------------------------------------------------- */

const ntg_entity_type NTG_ENTITY_LOOP = {
    .__parent = &NTG_ENTITY,
    ._size = sizeof(ntg_loop)
};

const ntg_entity_type NTG_ENTITY_TASK_RUNNER = {
    .__parent = &NTG_ENTITY,
    ._size = sizeof(ntg_task_runner)
};

const ntg_entity_type NTG_ENTITY_PLATFORM = {
    .__parent = &NTG_ENTITY,
    ._size = sizeof(ntg_platform)
};
