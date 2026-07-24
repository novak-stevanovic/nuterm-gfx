#ifndef NTG_OBJECT_DECORATOR_H
#define NTG_OBJECT_DECORATOR_H

#include "shared/ntg_shared.h"
#include "base/ntg_xy.h"

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* BORDER & PADDING */
/* ------------------------------------------------------ */

enum ntg_object_dcr_enable
{
    NTG_OBJECT_DCR_ENABLE_MIN = 0,
    NTG_OBJECT_DCR_ENABLE_NAT,
    NTG_OBJECT_DCR_ENABLE_ALWAYS
};

struct ntg_border_opts
{
    ntg_object_dcr_enable enable;
    struct ntg_insets pref_size;
    const struct ntg_border_style* style;
};

struct ntg_padding_opts
{
    ntg_object_dcr_enable enable;
    struct ntg_insets pref_size;
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* BORDER & PADDING */
/* ------------------------------------------------------ */


NTG_API struct ntg_border_opts
ntg_border_opts_def();

/* ------------------------------------------------------ */


NTG_API bool
ntg_border_opts_are_eql(
        const struct ntg_border_opts* opts1,
        const struct ntg_border_opts* opts2);


NTG_API struct ntg_padding_opts
ntg_padding_opts_def();

/* ------------------------------------------------------ */


NTG_API bool
ntg_padding_opts_are_eql(
        const struct ntg_padding_opts* opts1,
        const struct ntg_padding_opts* opts2);

#endif // NTG_OBJECT_DECORATOR_H
