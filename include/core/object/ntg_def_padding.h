#ifndef _NTG_DEF_PADDING_H_
#define _NTG_DEF_PADDING_H_

#include "core/object/ntg_padding.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_def_padding
{
    ntg_padding __base;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_def_padding* ntg_def_padding_new(ntg_entity_system* system);
void ntg_def_padding_init(ntg_def_padding* def_padding);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_def_padding_deinit_fn(ntg_entity* entity);

void _ntg_def_padding_draw_fn(
        const ntg_object* _padding,
        void* _layout_data,
        struct ntg_xy size,
        ntg_temp_object_drawing* out_drawing,
        sarena* arena);

#endif // _NTG_DEF_PADDING_H_
