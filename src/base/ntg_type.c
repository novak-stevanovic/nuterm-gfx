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

const ntg_type NTG_TYPE_OBJECT = {
    .__parent = NULL,
    ._size = sizeof(ntg_object)
};

const ntg_type NTG_TYPE_BOX = {
    .__parent = &NTG_TYPE_OBJECT,
    ._size = sizeof(ntg_box)
};

const ntg_type NTG_TYPE_MAIN_PANEL = {
    .__parent = &NTG_TYPE_OBJECT,
    ._size = sizeof(ntg_main_panel)
};

const ntg_type NTG_TYPE_LABEL = {
    .__parent = &NTG_TYPE_OBJECT,
    ._size = sizeof(ntg_label)
};

const ntg_type NTG_TYPE_COLOR_BLOCK = {
    .__parent = &NTG_TYPE_OBJECT,
    ._size = sizeof(ntg_color_block)
};

const ntg_type NTG_TYPE_PROG_BAR = {
    .__parent = &NTG_TYPE_OBJECT,
    ._size = sizeof(ntg_prog_bar)
};
