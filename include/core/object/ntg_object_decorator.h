#ifndef NTG_OBJECT_DECORATOR_H
#define NTG_OBJECT_DECORATOR_H

#include "shared/ntg_shared.h"
#include "base/ntg_xy.h"

/* ========================================================================== */
/* PUBLIC - TYPES AND FUNCTIONS */
/* ========================================================================== */

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

/* Creates default border options with the default style, zero preferred insets,
 * and minimum enable mode.
 *
 * RETURN VALUE:
 * The default `ntg_border_opts` value. */
NTG_API struct ntg_border_opts
ntg_border_opts_def();

/* ------------------------------------------------------ */

/* Compares two border option values. Pointer identity counts as equal;
 * otherwise a `NULL` value differs from a non-`NULL` value. */
NTG_API bool
ntg_border_opts_are_eq(
        const struct ntg_border_opts* opts1,
        const struct ntg_border_opts* opts2);

struct ntg_padding_opts
{
    ntg_object_dcr_enable enable;
    struct ntg_insets pref_size;
};

/* Creates default padding options with zero preferred insets and minimum enable
 * mode.
 *
 * RETURN VALUE:
 * The default `ntg_padding_opts` value. */
NTG_API struct ntg_padding_opts
ntg_padding_opts_def();

/* ------------------------------------------------------ */

/* Compares two padding option values. Pointer identity counts as equal;
 * otherwise a `NULL` value differs from a non-`NULL` value. */
NTG_API bool
ntg_padding_opts_are_eq(
        const struct ntg_padding_opts* opts1,
        const struct ntg_padding_opts* opts2);

#endif // NTG_OBJECT_DECORATOR_H
