#include "base/entity/_ntg_entity_type.h"
#include "core/loop/ntg_loop.h"
#include "core/loop/ntg_taskmaster.h"
#include "core/object/ntg_border_box.h"
#include "core/object/ntg_box.h"
#include "core/object/ntg_color_block.h"
#include "core/object/ntg_def_border.h"
#include "core/object/ntg_def_padding.h"
#include "core/object/ntg_label.h"
#include "core/object/ntg_object.h"
#include "core/object/ntg_prog_bar.h"
#include "core/object/ntg_padding.h"
#include "core/renderer/ntg_def_renderer.h"
#include "core/renderer/ntg_renderer.h"
#include "core/scene/ntg_def_scene.h"
#include "core/scene/ntg_scene.h"
#include "core/stage/ntg_def_stage.h"
#include "core/stage/ntg_stage.h"

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

bool ntg_entity_is(const ntg_entity_type* type, const ntg_entity_type* ancestor)
{
    return (type == ancestor);
}

/* -------------------------------------------------------------------------- */
/* Base                                                                        */
/* -------------------------------------------------------------------------- */

const ntg_entity_type NTG_ENTITY_DEFAULT = {
    .__parent = NULL,
    ._size = sizeof(ntg_entity)
};

/* -------------------------------------------------------------------------- */
/* NTG_OBJECT hierarchy                                                       */
/* -------------------------------------------------------------------------- */

const ntg_entity_type NTG_ENTITY_OBJECT = {
    .__parent = &NTG_ENTITY_DEFAULT,
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
/* NTG_SCENE                                                                  */
/* -------------------------------------------------------------------------- */

const ntg_entity_type NTG_ENTITY_SCENE = {
    .__parent = &NTG_ENTITY_DEFAULT,
    ._size = sizeof(ntg_scene)
};

const ntg_entity_type NTG_ENTITY_DEF_SCENE = {
    .__parent = &NTG_ENTITY_SCENE,
    ._size = sizeof(ntg_def_scene)
};

/* -------------------------------------------------------------------------- */
/* NTG_STAGE                                                                  */
/* -------------------------------------------------------------------------- */

const ntg_entity_type NTG_ENTITY_STAGE = {
    .__parent = &NTG_ENTITY_DEFAULT,
    ._size = sizeof(ntg_stage)
};

const ntg_entity_type NTG_ENTITY_DEF_STAGE = {
    .__parent = &NTG_ENTITY_STAGE,
    ._size = sizeof(ntg_def_stage)
};

/* -------------------------------------------------------------------------- */
/* NTG_RENDERER                                                               */
/* -------------------------------------------------------------------------- */

const ntg_entity_type NTG_ENTITY_RENDERER = {
    .__parent = &NTG_ENTITY_DEFAULT,
    ._size = sizeof(ntg_renderer)
};

const ntg_entity_type NTG_ENTITY_DEF_RENDERER = {
    .__parent = &NTG_ENTITY_RENDERER,
    ._size = sizeof(ntg_def_renderer)
};

/* -------------------------------------------------------------------------- */
/* NTG_LOOP                                                                   */
/* -------------------------------------------------------------------------- */

const ntg_entity_type NTG_ENTITY_LOOP = {
    .__parent = &NTG_ENTITY_DEFAULT,
    ._size = sizeof(ntg_entity)
};

/* -------------------------------------------------------------------------- */
/* NTG_TASKMASTER                                                             */
/* -------------------------------------------------------------------------- */

const ntg_entity_type NTG_ENTITY_TASKMASTER = {
    .__parent = &NTG_ENTITY_DEFAULT,
    ._size = sizeof(ntg_taskmaster)
};
