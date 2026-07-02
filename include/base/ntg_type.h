#ifndef NTG_TYPE_H
#define NTG_TYPE_H

#include "shared/ntg_shared.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

struct ntg_type
{
    const ntg_type* __parent;
    size_t _size;
};

extern const ntg_type NTG_TYPE_OBJECT,
        NTG_TYPE_PROG_BAR, NTG_TYPE_COLOR_BLOCK,
        NTG_TYPE_LABEL, NTG_TYPE_BOX, NTG_TYPE_MAIN_PANEL;

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* Checks whether `type` is `ancestor` or derives from it through the type parent chain. */

/* RETURN VALUE: `true` when the relationship holds. Two `NULL` pointers also return `true`; */
/*               exactly one `NULL` pointer returns `false`. */
bool ntg_type_instance_of(const ntg_type* type, const ntg_type* ancestor);
/* Compares two type descriptors by identity. */

/* RETURN VALUE: `true` when both pointers designate the same type, including two `NULL` */
/*               pointers; otherwise `false`. */
bool ntg_type_are_equal(const ntg_type* type1, const ntg_type* type2);

#endif // NTG_TYPE_H
