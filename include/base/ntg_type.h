#ifndef NTG_TYPE_H
#define NTG_TYPE_H

#include "shared/ntg_shared.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_type
{
    struct
    {
        size_t size;
    } ro;

    struct
    {
        const ntg_type* parent;
    } priv;
};


NTG_API extern const ntg_type
        NTG_TYPE_OBJECT,
        NTG_TYPE_PROG_BAR,
        NTG_TYPE_CLR_BLOCK,
        NTG_TYPE_TEXT,
        NTG_TYPE_LABEL,
        NTG_TYPE_BUTTON,
        NTG_TYPE_BOX,
        NTG_TYPE_MAIN_PANEL;

NTG_API extern const ntg_type NTG_TYPE_SCENE;

NTG_API extern const ntg_type NTG_TYPE_STAGE;

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */


NTG_API bool
ntg_type_instance_of(const ntg_type* type, const ntg_type* ancestor);

/* ------------------------------------------------------ */


NTG_API bool
ntg_type_are_eql(const ntg_type* type1, const ntg_type* type2);

#endif // NTG_TYPE_H
