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

const ntg_type NTG_TYPE_ENTIY = {
    .ro.size = sizeof(ntg_entity),
    .ro.parent = &NTG_TYPE_ENTITY
};

/* ------------------------------------------------------ */

const ntg_type NTG_TYPE_OBJECT = {
    .ro.size = sizeof(ntg_object),
    .ro.parent = &NTG_TYPE_ENTITY
};

/* ------------------------------------------------------ */

const ntg_type NTG_TYPE_BOX = {
    .ro.size = sizeof(ntg_box),
    .ro.parent = &NTG_TYPE_OBJECT
};

const ntg_type NTG_TYPE_MAIN_PANEL = {
    .ro.size = sizeof(ntg_main_panel),
    .ro.parent = &NTG_TYPE_OBJECT
};

const ntg_type NTG_TYPE_TEXT = {
    .ro.size = sizeof(ntg_text),
    .ro.parent = &NTG_TYPE_OBJECT
};

const ntg_type NTG_TYPE_LABEL = {
    .ro.size = sizeof(ntg_label),
    .ro.parent = &NTG_TYPE_TEXT
};

const ntg_type NTG_TYPE_BUTTON = {
    .ro.size = sizeof(ntg_button),
    .ro.parent = &NTG_TYPE_TEXT
};

const ntg_type NTG_TYPE_CLR_BLOCK = {
    .ro.size = sizeof(ntg_clr_block),
    .ro.parent = &NTG_TYPE_OBJECT
};

const ntg_type NTG_TYPE_PROG_BAR = {
    .ro.size = sizeof(ntg_prog_bar),
    .ro.parent = &NTG_TYPE_OBJECT
};

/* ------------------------------------------------------ */

const ntg_type NTG_TYPE_SCENE = {
    .ro.size = sizeof(ntg_scene),
    .ro.parent = &NTG_TYPE_ENTITY
};

/* ------------------------------------------------------ */

const ntg_type NTG_TYPE_STAGE = {
    .ro.size = sizeof(ntg_stage),
    .ro.parent = &NTG_TYPE_ENTITY
};

/* ------------------------------------------------------ */

const ntg_type NTG_TYPE_RENDERER = {
    .ro.size = sizeof(ntg_renderer),
    .ro.parent = &NTG_TYPE_ENTITY
};

const ntg_type NTG_TYPE_DB_RENDERER = {
    .ro.size = sizeof(ntg_db_renderer),
    .ro.parent = &NTG_TYPE_RENDERER
};

/* ------------------------------------------------------ */

const ntg_type NTG_TYPE_ANCHOR_POLICY = {
    .ro.size = sizeof(ntg_anchor_policy),
    .ro.parent = &NTG_TYPE_ENTITY
};

const ntg_type NTG_TYPE_BORDER_STYLE = {
    .ro.size = sizeof(ntg_border_style),
    .ro.parent = &NTG_TYPE_ENTITY
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
