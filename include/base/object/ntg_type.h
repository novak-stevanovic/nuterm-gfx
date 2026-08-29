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
        const ntg_type* parent;
    } ro;
};

/* ------------------------------------------------------ */

NTG_API extern const ntg_type NTG_TYPE_OBJECT;

NTG_API extern const ntg_type NTG_TYPE_WIDGET;

NTG_API extern const ntg_type NTG_TYPE_BOX;
NTG_API extern const ntg_type NTG_TYPE_PANEL;
NTG_API extern const ntg_type NTG_TYPE_TEXT_WGT;
NTG_API extern const ntg_type NTG_TYPE_LABEL;
NTG_API extern const ntg_type NTG_TYPE_BUTTON;
NTG_API extern const ntg_type NTG_TYPE_CLR_BLOCK;
NTG_API extern const ntg_type NTG_TYPE_PROG_BAR;

NTG_API extern const ntg_type NTG_TYPE_SCENE;
NTG_API extern const ntg_type NTG_TYPE_FCS_MANAGER;

NTG_API extern const ntg_type NTG_TYPE_STAGE;

NTG_API extern const ntg_type NTG_TYPE_RENDERER;
NTG_API extern const ntg_type NTG_TYPE_DB_RENDERER;

NTG_API extern const ntg_type NTG_TYPE_ANCHOR_POLICY;
NTG_API extern const ntg_type NTG_TYPE_FLOAT;
NTG_API extern const ntg_type NTG_TYPE_SIDEFLOAT;

NTG_API extern const ntg_type NTG_TYPE_BORDER_STYLE;
NTG_API extern const ntg_type NTG_TYPE_BORDER_9X;

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API bool
ntg_type_instanceof(const ntg_type* type, const ntg_type* ancestor);

/* ------------------------------------------------------ */

NTG_API bool
ntg_type_are_eql(const ntg_type* type1, const ntg_type* type2);

#endif // NTG_TYPE_H
