#include "ntg.h"
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
    .ro.size = sizeof(ntg_object),
    .ro.parent = NULL
};

/* ------------------------------------------------------ */

const ntg_type NTG_TYPE_WIDGET = {
    .ro.size = sizeof(ntg_widget),
    .ro.parent = &NTG_TYPE_OBJECT
};

/* ------------------------------------------------------ */

const ntg_type NTG_TYPE_BOX = {
    .ro.size = sizeof(ntg_box),
    .ro.parent = &NTG_TYPE_WIDGET
};

const ntg_type NTG_TYPE_PANEL = {
    .ro.size = sizeof(ntg_panel),
    .ro.parent = &NTG_TYPE_WIDGET
};

const ntg_type NTG_TYPE_TEXT_WGT = {
    .ro.size = sizeof(ntg_text_wgt),
    .ro.parent = &NTG_TYPE_WIDGET
};

const ntg_type NTG_TYPE_LABEL = {
    .ro.size = sizeof(ntg_label),
    .ro.parent = &NTG_TYPE_TEXT_WGT
};

const ntg_type NTG_TYPE_BUTTON = {
    .ro.size = sizeof(ntg_button),
    .ro.parent = &NTG_TYPE_TEXT_WGT
};

const ntg_type NTG_TYPE_CLR_BLOCK = {
    .ro.size = sizeof(ntg_clr_block),
    .ro.parent = &NTG_TYPE_WIDGET
};

const ntg_type NTG_TYPE_PROG_BAR = {
    .ro.size = sizeof(ntg_prog_bar),
    .ro.parent = &NTG_TYPE_WIDGET
};

/* ------------------------------------------------------ */

const ntg_type NTG_TYPE_SCENE = {
    .ro.size = sizeof(ntg_scene),
    .ro.parent = &NTG_TYPE_OBJECT
};

const ntg_type NTG_TYPE_FCS_MANAGER = {
    .ro.size = sizeof(ntg_fcs_manager),
    .ro.parent = &NTG_TYPE_OBJECT
};

/* ------------------------------------------------------ */

const ntg_type NTG_TYPE_STAGE = {
    .ro.size = sizeof(ntg_stage),
    .ro.parent = &NTG_TYPE_OBJECT
};

/* ------------------------------------------------------ */

const ntg_type NTG_TYPE_RENDERER = {
    .ro.size = sizeof(ntg_renderer),
    .ro.parent = &NTG_TYPE_OBJECT
};

const ntg_type NTG_TYPE_DB_RENDERER = {
    .ro.size = sizeof(ntg_db_renderer),
    .ro.parent = &NTG_TYPE_RENDERER
};

/* ------------------------------------------------------ */

const ntg_type NTG_TYPE_ANCHOR_POLICY = {
    .ro.size = sizeof(ntg_anchor_policy),
    .ro.parent = &NTG_TYPE_OBJECT
};

const ntg_type NTG_TYPE_FLOAT = {
    .ro.size = sizeof(ntg_float),
    .ro.parent = &NTG_TYPE_ANCHOR_POLICY
};

const ntg_type NTG_TYPE_SIDEFLOAT = {
    .ro.size = sizeof(ntg_sidefloat),
    .ro.parent = &NTG_TYPE_ANCHOR_POLICY
};

const ntg_type NTG_TYPE_BORDER_STYLE = {
    .ro.size = sizeof(ntg_border_style),
    .ro.parent = &NTG_TYPE_OBJECT
};

const ntg_type NTG_TYPE_BORDER_9X = {
    .ro.size = sizeof(ntg_border_9x),
    .ro.parent = &NTG_TYPE_BORDER_STYLE
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

bool ntg_type_instanceof(const ntg_type* type, const ntg_type* ancestor)
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

        it = it->ro.parent;
    }

    return false;
}

/* ------------------------------------------------------ */

bool ntg_type_are_eql(const ntg_type* type1, const ntg_type* type2)
{
    return type1 == type2;
}
