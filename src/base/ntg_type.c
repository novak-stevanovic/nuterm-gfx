#include "ntg.h"
#include "core/loop/ntg_loop_internal.h"
#include "shared/ntg_shared_internal.h"

bool ntg_type_instance_of(const ntg_type* type, const ntg_type* ancestor)
{
    const ntg_type* it = type;

    while (it != NULL)
    {
        if (it == ancestor) return true;

        it = it->__parent;
    }

    return false;
}

bool ntg_type_are_equal(const ntg_type* type1, const ntg_type* type2)
{
    return (type1 == type2);
}

/* -------------------------------------------------------------------------- */
/* Base */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* NTG_OBJECT */
/* -------------------------------------------------------------------------- */

const ntg_type NTG_OBJECT = {
    .__parent = NULL,
    ._size = sizeof(ntg_object)
};

// const ntg_type NTG_BOX = {
//     .__parent = &NTG_WIDGET,
//     ._size = sizeof(ntg_box)
// };
// 
// const ntg_type NTG_BBOX = {
//     .__parent = &NTG_WIDGET,
//     ._size = sizeof(ntg_bbox)
// };
// 
// const ntg_type NTG_LABEL = {
//     .__parent = &NTG_WIDGET,
//     ._size = sizeof(ntg_label)
// };
// 
// const ntg_type NTG_CBLOCK = {
//     .__parent = &NTG_WIDGET,
//     ._size = sizeof(ntg_cblock)
// };
// 
// const ntg_type NTG_PBAR = {
//     .__parent = &NTG_WIDGET,
//     ._size = sizeof(ntg_pbar)
// };

/* -------------------------------------------------------------------------- */
/* NTG_SCENE */
/* -------------------------------------------------------------------------- */

const ntg_type NTG_SCENE = {
    .__parent = NULL,
    ._size = sizeof(ntg_scene)
};

/* -------------------------------------------------------------------------- */
/* NTG_STAGE */
/* -------------------------------------------------------------------------- */

const ntg_type NTG_STAGE = {
    .__parent = NULL,
    ._size = sizeof(ntg_stage)
};

/* -------------------------------------------------------------------------- */
/* NTG_RENDERER */
/* -------------------------------------------------------------------------- */

const ntg_type NTG_RENDERER = {
    .__parent = NULL,
    ._size = sizeof(ntg_renderer)
};

const ntg_type NTG_DEF_RENDERER = {
    .__parent = &NTG_RENDERER,
    ._size = sizeof(ntg_def_renderer)
};

/* -------------------------------------------------------------------------- */
/* NTG_LOOP */
/* -------------------------------------------------------------------------- */

const ntg_type NTG_LOOP = {
    .__parent = NULL,
    ._size = sizeof(ntg_loop)
};
