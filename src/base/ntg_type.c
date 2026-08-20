#include "ntg.h"
// #include "core/loop/ntg_loop_internal.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

const ntg_type NTG_TYPE_OBJECT = {
    .priv.parent = NULL,
    .ro.size = sizeof(ntg_object)
};

const ntg_type NTG_TYPE_BOX = {
    .priv.parent = &NTG_TYPE_OBJECT,
    .ro.size = sizeof(ntg_box)
};

const ntg_type NTG_TYPE_MAIN_PANEL = {
    .priv.parent = &NTG_TYPE_OBJECT,
    .ro.size = sizeof(ntg_main_panel)
};

const ntg_type NTG_TYPE_TEXT = {
    .priv.parent = &NTG_TYPE_OBJECT,
    .ro.size = sizeof(ntg_text)
};

const ntg_type NTG_TYPE_LABEL = {
    .priv.parent = &NTG_TYPE_TEXT,
    .ro.size = sizeof(ntg_label)
};

const ntg_type NTG_TYPE_BUTTON = {
    .priv.parent = &NTG_TYPE_TEXT,
    .ro.size = sizeof(ntg_button)
};

const ntg_type NTG_TYPE_CLR_BLOCK = {
    .priv.parent = &NTG_TYPE_OBJECT,
    .ro.size = sizeof(ntg_clr_block)
};

const ntg_type NTG_TYPE_PROG_BAR = {
    .priv.parent = &NTG_TYPE_OBJECT,
    .ro.size = sizeof(ntg_prog_bar)
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

bool ntg_type_instance_of(const ntg_type* type, const ntg_type* ancestor)
{
    if(!type && !ancestor)
        return true;

    if(!type || !ancestor)
        return false;

    const ntg_type* it = type;

    while(it)
    {
        if(it == ancestor)
            return true;

        it = it->priv.parent;
    }

    return false;
}

bool ntg_type_are_eql(const ntg_type* type1, const ntg_type* type2)
{
    return (type1 == type2);
}
