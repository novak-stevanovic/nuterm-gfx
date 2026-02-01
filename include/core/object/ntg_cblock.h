#ifndef NTG_CBLOCK_H
#define NTG_CBLOCK_H

#include "shared/ntg_shared.h"
#include "core/object/ntg_object.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_cblock
{
    ntg_object __base;
    nt_color __color;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_cblock_init(ntg_cblock* cblock);
void ntg_cblock_deinit(ntg_cblock* block);

void ntg_cblock_set_color(ntg_cblock* cblock, nt_color color);

#endif // NTG_CBLOCK_H
